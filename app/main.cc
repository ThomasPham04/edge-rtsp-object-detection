#include "rtsp_reader/rtsp_reader.h"
#include "decoder/hw_decoder.h"
#include "encoder/hw_encoder.h"
#include "AI/detector.h"
#include "sys/sys_init.h"
#include <csignal>
#include <unistd.h>
#include <atomic>
#include <vector>
#include <unordered_map>
#include "stream.h"
#include "BYTETracker.h"
#include "cvi_draw_rect.h"

std::unordered_map<std::string, PAYLOAD_TYPE_E> decode_type = {
        {"H.264", PT_H264},
        {"H.265", PT_H265}
};
std::atomic<bool> stop(false);  

void int_handler(int signal){
    stop = true;
}
int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: app_AI_decode <model_path> [rtsp_url]\n";
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
    std::string rtspUrl = (argc >= 3) ? std::string(argv[2]) : ip;
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

    SystemInit::init(srcWidth,srcHeight);
    rtspServer ser;
    ser.init(8854, 4);
    rtspSession *session = ser.createSession("cam1", RTSP_VIDEO_H264);

    PAYLOAD_TYPE_E type = decode_type[codecType];

    HardwareDecoder decoder(srcWidth, srcHeight, type);
    HardwareEncoder encoder(srcWidth, srcHeight, type);
    AIDetection detector(srcWidth, srcHeight);
    CVI_TDL_SUPPORTED_MODEL_E model = CVI_TDL_SUPPORTED_MODEL_YOLOV8_DETECTION;
    if (!detector.openModel(argv[1], model)) {
        std::cerr << "Failed to open model: " << argv[1] << "\n";
        reader.close();
        return -1;
    }
    detector.setThresholds(model, 0.5f, 0.5f);
    detector.ensureImageProcessor();
    AVPacket pkt;
    VIDEO_FRAME_INFO_S frame;
    VENC_STREAM_S stStream;
    cvtdl_object_t obj = {0}; 
    
    while (reader.readPacket(pkt)) {
            // Reduce log noise to save CPU and avoid flooding
            // std::cout   << "\nPacket size: " << pkt.size
            //             << " PTS: " << pkt.pts
            //             << " Stream index: " << pkt.stream_index << "\n";
        if (!decoder.sendPacket(pkt.data, pkt.size, pkt.pts)){
            std::cerr << "Failed to send packet to decoder\n";
            av_packet_unref(&pkt);
            continue;
        }
        if (decoder.getFrame(&frame)) {
            detector.objDectection(argv[1], &frame, model, &obj, 0.5f);

            // Hàm clamp giúp giới hạn giá trị nằm trong khoảng [low, high]
            auto clamp = [](float v, float low, float high) {
                return std::max(low, std::min(v, high));
            };

            std::vector<byte_track::Object> detected_objects;
            for (uint32_t i = 0; i < obj.size; i++) {
                if (obj.info[i].classes == 0) {  // Only person
                    float x1 = obj.info[i].bbox.x1;
                    float y1 = obj.info[i].bbox.y1;
                    float x2 = obj.info[i].bbox.x2;
                    float y2 = obj.info[i].bbox.y2;

                    // Tính width, height đúng
                    float width = x2 - x1;
                    float height = y2 - y1;

                    
                    x1 = clamp(x1, 0.f, (float)(srcWidth - 1));
                    y1 = clamp(y1, 0.f, (float)(srcHeight - 1));
                    width = std::min(width, (float)(srcWidth - x1));
                    height = std::min(height, (float)(srcHeight - y1));

                    byte_track::Rect<float> rect(x1, y1, width, height);
                    detected_objects.emplace_back(rect, 0, obj.info[i].bbox.score);
                }
            }

            auto tracks = tracker.update(detected_objects);
            // std::cout << "Tracked People:\n";
            for (const auto& track : tracks) {
                const auto& rect = track->getRect();  
                std::cout << "ID: " << track->getTrackId()
                        << " Box: x=" << rect.x()
                        << ", y=" << rect.y()
                        << ", width=" << rect.width()
                        << ", height=" << rect.height() << "\n";

                cvtdl_object_t obj_meta = {0};
                cvtdl_service_brush_t brushi;
                brushi.color.r = 255;
                brushi.color.g = 255;
                brushi.color.b = 255;
                brushi.size = 4;

                obj_meta.size = 1;
                obj_meta.rescale_type = meta_rescale_type_e::RESCALE_CENTER;

                
                obj_meta.info = (cvtdl_object_info_t *)malloc(sizeof(cvtdl_object_info_t) * obj_meta.size);
                if (!obj_meta.info) {
                    std::cerr << "Memory allocation failed for obj_meta.info\n";
                    continue;  
                }

                obj_meta.info[0].bbox.x1 = rect.x();
                obj_meta.info[0].bbox.y1 = rect.y();
                obj_meta.info[0].bbox.x2 = rect.x() + rect.width();
                obj_meta.info[0].bbox.y2 = rect.y() + rect.height();

                CVI_TDL_ObjectDrawRect(&obj_meta, &frame, false, brushi);

                
                free(obj_meta.info);
            }


            
           if (encoder.isStarted() && encoder.sendFrame(&frame)) {

            
            VENC_CHN_STATUS_S stStat;
            if (CVI_VENC_QueryStatus(encoder.getVencChn(), &stStat) != CVI_SUCCESS) {
                std::cerr << "CVI_VENC_QueryStatus failed\n";
                decoder.releaseFrame(&frame);
                continue;
            }

            if (stStat.u32CurPacks == 0) {
                std::cerr << "No encoded data available for this frame\n";
                decoder.releaseFrame(&frame);
                continue;
            }

            
            memset(&stStream, 0, sizeof(stStream));
            stStream.pstPack = (VENC_PACK_S*)malloc(sizeof(VENC_PACK_S) * stStat.u32CurPacks);
            if (!stStream.pstPack) {
                std::cerr << "malloc failed for pstPack\n";
                decoder.releaseFrame(&frame);
                continue;
            }

            
            if (encoder.getStream(&stStream)) {
                for (uint32_t i = 0; i < stStream.u32PackCount; i++) {
                    VENC_PACK_S *ppack = &stStream.pstPack[i];
                    const uint8_t* sendPtr = ppack->pu8Addr + ppack->u32Offset;
                    uint32_t sendLen = ppack->u32Len - ppack->u32Offset;
                    uint64_t pts = ppack->u64PTS;

                    if (!session->writeFrame(sendPtr, sendLen, pts)) {
                        std::cerr << "Failed to write frame to RTSP\n";
                    }
                }
                encoder.releaseStream(&stStream);
            } else {
                std::cerr << "No stream data available yet\n";
            }

            free(stStream.pstPack);
            stStream.pstPack = NULL;

        } else if (!encoder.isStarted()) {
            std::cerr << "Encoder not started; skipping frame\n";
        } else {
            std::cerr << "Encoder can't send frame\n";
        }

            decoder.releaseFrame(&frame);
        }

        CVI_TDL_Free(&obj);
        av_packet_unref(&pkt);

        if(stop) break;
    }
    reader.close();
    return 0;
}