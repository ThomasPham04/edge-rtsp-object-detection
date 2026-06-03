#include "rtsp_reader/rtsp_reader.h"
#include "decoder/hw_decoder.h"
#include "encoder/hw_encoder.h"
#include "AI/detector.h"
#include "sys/sys_init.h"
#include <csignal>
#include <unistd.h>
#include <algorithm>
#include <atomic>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <unordered_map>
#include "BYTETracker.h"
#include "cvi_draw_rect.h"
#include <chrono>

const std::unordered_map<std::string, PAYLOAD_TYPE_E> decode_type = {
        {"H.264", PT_H264},
        {"H.265", PT_H265}
};
std::atomic<bool> stop(false);  

void int_handler(int signal){
    if (signal == SIGINT)
        stop = true;
}
int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: app_AI_decode <model_path> [rtsp_url]\n"
                  << "       or set RTSP_URL when rtsp_url is omitted\n";
        return -1;
    }
    
    byte_track::BYTETracker tracker(
        30,     // frame_rate
        30,     // track_buffer
        0.5,    // track_thresh
        0.6,    // high_thresh
        0.8     // match_thresh
    );
    RtspReader reader;
    signal(SIGINT, int_handler);
    std::cout << "-------------------Hardware Decoder-------------------\n";
    const char* envRtspUrl = std::getenv("RTSP_URL");
    std::string rtspUrl = (argc >= 3) ? std::string(argv[2]) : (envRtspUrl ? std::string(envRtspUrl) : "");
    if (rtspUrl.empty()) {
        std::cerr << "Missing RTSP URL. Pass it as argv[2] or set RTSP_URL.\n";
        return -1;
    }

    if (!reader.open(rtspUrl.c_str())){
        std::cerr << "Failed to open\n";
        return -1;
    }  else {
        std::cout << "Open successfully\n";
    }

    int srcWidth = reader.getVideoWidth();
    int srcHeight = reader.getVideoHeight();
    std::string codecType = reader.getCodecType();
    std::cout << "Video Width: " << srcWidth << ", Height: " << srcHeight << ", Type: " << codecType << "\n";

    auto decodeTypeIt = decode_type.find(codecType);
    if (decodeTypeIt == decode_type.end()) {
        std::cerr << "Unsupported codec: " << codecType << "\n";
        reader.close();
        return -1;
    }
    PAYLOAD_TYPE_E decodeType = decodeTypeIt->second;
    PAYLOAD_TYPE_E encodeType = PT_H264;

    if (!SystemInit::init(srcWidth,srcHeight)) {
        std::cerr << "System initialization failed\n";
        reader.close();
        return -1;
    }

    rtspServer ser;
    if (!ser.init(8854, 4)) {
        std::cerr << "RTSP server initialization failed\n";
        reader.close();
        return -1;
    }

    rtspSession *session = ser.createSession("cam1", RTSP_VIDEO_H264);
    if (!session) {
        std::cerr << "Failed to create RTSP session\n";
        reader.close();
        return -1;
    }

    HardwareDecoder decoder(srcWidth, srcHeight, decodeType);
    HardwareEncoder encoder(srcWidth, srcHeight, encodeType);
    if (!decoder.isStarted()) {
        std::cerr << "Decoder initialization failed\n";
        reader.close();
        return -1;
    }
    if (!encoder.isStarted()) {
        std::cerr << "Encoder initialization failed\n";
        reader.close();
        return -1;
    }

    AIDetection detector(srcWidth, srcHeight);
    CVI_TDL_SUPPORTED_MODEL_E model = CVI_TDL_SUPPORTED_MODEL_YOLOV8_DETECTION;
    if (!detector.openModel(argv[1], model)) {
        std::cerr << "Failed to open model: " << argv[1] << "\n";
        reader.close();
        return -1;
    }
    detector.setThresholds(model, 0.5f, 0.5f);
    if (!detector.ensureImageProcessor()) {
        std::cerr << "Failed to initialize image processor\n";
        reader.close();
        return -1;
    }
    AVPacket pkt;
    VIDEO_FRAME_INFO_S frame;
    cvtdl_object_t obj;
    cvtdl_service_brush_t brushi;
    brushi.color.r = 255;
    brushi.color.g = 255;
    brushi.color.b = 255;
    brushi.size = 4;
    memset(&obj, 0, sizeof(obj));

    int frame_count = 0;
    auto start_time = std::chrono::steady_clock::now();

    while (reader.readPacket(pkt)) {
        if (!decoder.sendPacket(pkt.data, pkt.size, pkt.pts)) {
            std::cerr << "Failed to send packet to decoder\n";
            av_packet_unref(&pkt);
            if (stop) break;
            continue;
        }

        if (decoder.getFrame(&frame)) {
            detector.objDectection(&frame,&obj);

            auto clamp = [](float v, float low, float high) {
                return std::max(low, std::min(v, high));
            };

            std::vector<byte_track::Object> detected_objects;
            for (uint32_t i = 0; i < obj.size; i++) {
                if (obj.info[i].classes != 0) {
                    continue;
                }

                float x1 = obj.info[i].bbox.x1;
                float y1 = obj.info[i].bbox.y1;
                float x2 = obj.info[i].bbox.x2;
                float y2 = obj.info[i].bbox.y2;

                x1 = clamp(x1, 0.f, (float)(srcWidth - 1));
                y1 = clamp(y1, 0.f, (float)(srcHeight - 1));
                x2 = clamp(x2, 0.f, (float)(srcWidth - 1));
                y2 = clamp(y2, 0.f, (float)(srcHeight - 1));
                if (x2 <= x1 || y2 <= y1) {
                    continue;
                }

                float width = x2 - x1;
                float height = y2 - y1;

                byte_track::Rect<float> rect(x1, y1, width, height);
                detected_objects.emplace_back(rect, 0, obj.info[i].bbox.score);
            }

            auto tracks = tracker.update(detected_objects);

            for (const auto& track : tracks) {
                const auto& rect = track->getRect();  
                std::cout << "ID: " << track->getTrackId()
                        << " Box: x=" << rect.x()
                        << ", y=" << rect.y()
                        << ", width=" << rect.width()
                        << ", height=" << rect.height() << "\n";

                cvtdl_object_t obj_meta;
                memset(&obj_meta, 0, sizeof(obj_meta));
                cvtdl_object_info_t obj_info;
                memset(&obj_info, 0, sizeof(obj_info));

                obj_meta.size = 1;
                obj_meta.rescale_type = meta_rescale_type_e::RESCALE_CENTER;
                obj_meta.info = &obj_info;

                obj_meta.info[0].bbox.x1 = rect.x();
                obj_meta.info[0].bbox.y1 = rect.y();
                obj_meta.info[0].bbox.x2 = rect.x() + rect.width();
                obj_meta.info[0].bbox.y2 = rect.y() + rect.height();

                CVI_TDL_ObjectDrawRect(&obj_meta, &frame, false, brushi);
            }

            if (encoder.isStarted() && encoder.sendFrame(&frame)) {
                VENC_CHN_STATUS_S stStat;
                memset(&stStat, 0, sizeof(stStat));
                if (CVI_VENC_QueryStatus(encoder.getVencChn(), &stStat) != CVI_SUCCESS) {
                    std::cerr << "CVI_VENC_QueryStatus failed\n";
                } else if (stStat.u32CurPacks == 0) {
                    std::cerr << "No encoded data available for this frame\n";
                } else {
                    VENC_STREAM_S stStream;
                    memset(&stStream, 0, sizeof(stStream));
                    std::vector<VENC_PACK_S> packs(stStat.u32CurPacks);
                    stStream.pstPack = packs.data();

                    if (encoder.getStream(&stStream)) {
                        for (uint32_t i = 0; i < stStream.u32PackCount; i++) {
                            VENC_PACK_S *ppack = &stStream.pstPack[i];
                            const uint8_t* sendPtr = ppack->pu8Addr;
                            uint32_t sendLen = ppack->u32Len;
                            uint64_t pts = ppack->u64PTS;

                            if (!session->writeFrame(sendPtr, sendLen, pts)) {
                                std::cerr << "Failed to write frame to RTSP\n";
                            }
                        }
                        encoder.releaseStream(&stStream);
                    } else {
                        std::cerr << "No stream data available yet\n";
                    }
                }
            } else if (!encoder.isStarted()) {
                std::cerr << "Encoder not started; skipping frame\n";
            } else {
                std::cerr << "Encoder can't send frame\n";
            }

            decoder.releaseFrame(&frame);

            frame_count++;
            if (frame_count % 30 == 0) {
                auto end_time = std::chrono::steady_clock::now();
                std::chrono::duration<double> elapsed = end_time - start_time;
                std::cout << "FPS: " << 30.0 / elapsed.count() << "\n";
                start_time = end_time;
            }
        }

        CVI_TDL_Free(&obj);
        memset(&obj, 0, sizeof(obj));
        av_packet_unref(&pkt);

        if (stop) break;
    }
    reader.close();
    return 0;
}
