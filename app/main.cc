#include "rtsp_reader/rtsp_reader.h"
#include "decoder/hw_decoder.h"
#include "encoder/hw_encoder.h"
#include "AI/detector.h"
#include "sys/sys_init.h"
#include <csignal>
#include <unistd.h>
#include <atomic>
#include <vector>
#include "stream.h"
#include "BYTETracker.h"

std::unordered_map<std::string, PAYLOAD_TYPE_E> decode_type = {
        {"H.264", PT_H264},
        {"H.265", PT_H265}
};
std::atomic<bool> stop(false);  

void int_handler(int signal){
    stop = true;
}
int main(int argc, char* argv[]) {
    
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
    if (!reader.open(ip.c_str())){
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


    CVI_RTSP_TRACK videoTrack = nullptr; 

    PAYLOAD_TYPE_E type = decode_type[codecType];

    HardwareDecoder decoder(srcWidth, srcHeight, type);
    HardwareEncoder encoder(srcWidth, srcHeight, type);
    AIDetection detector(srcWidth, srcHeight);
    AVPacket pkt;
    VIDEO_FRAME_INFO_S frame;
    CVI_TDL_SUPPORTED_MODEL_E model = CVI_TDL_SUPPORTED_MODEL_YOLOV8_DETECTION;
    cvtdl_object_t obj = {0}; 
    
    while (!stop && reader.readPacket(pkt)) {
        std::cout   << "\nPacket size: " << pkt.size
                    << " PTS: " << pkt.pts
                    << " Stream index: " << pkt.stream_index << "\n";
        if (!decoder.sendPacket(pkt.data, pkt.size, pkt.pts)){
            std::cerr << "Failed to send packet to decoder\n";
            av_packet_unref(&pkt);
            continue;
        }
        if (decoder.getFrame(&frame)) {
            detector.objDectection(argv[1], &frame, model, &obj, 0.5);

            std::vector<byte_track::Object> detected_objects;
            for (uint32_t i = 0; i < obj.size; i++) {
                if (obj.info[i].classes == 0) {  // Only person
                    byte_track::Rect<float> rect(
                        obj.info[i].bbox.x1,
                        obj.info[i].bbox.y1,
                        obj.info[i].bbox.x2,
                        obj.info[i].bbox.y2
                    );
                    detected_objects.emplace_back(rect, 0, obj.info[i].bbox.score);
                }
            }

            auto tracks = tracker.update(detected_objects);
            std::cout << "Tracked People:\n";
            for (const auto& track : tracks) {
                const auto& rect = track->getRect();  // rect is of type Rect<float>
                std::cout << "ID: " << track->getTrackId()
                        << " Box: x=" << rect.x()
                        << ", y=" << rect.y()
                        << ", width=" << rect.width()
                        << ", height=" << rect.height() << "\n";
                cvitdl_service_handle_t handle;
                cvtdl_object_t obj_meta = {0};
                cvtdl_service_brush_t brushi;
                brushi.color.r = 255;
                brushi.color.g = 255;
                brushi.color.b = 255;
                brushi.size = 4;

                obj_meta.size = 1;
                obj_meta.rescale_type = meta_rescale_type_e::RESCALE_CENTER;
                obj_meta.info = (cvtdl_object_info_t *)malloc(sizeof(cvtdl_object_info_t) * obj_meta.size);
                obj_meta.info[0].bbox.x1 = rect.x();
                obj_meta.info[0].bbox.y1 = rect.y();
                obj_meta.info[0].bbox.x2 = rect.x() + rect.width();
                obj_meta.info[0].bbox.y2 = rect.y() + rect.height();
                CVI_TDL_Service_ObjectDrawRect(handle, &obj_meta, &frame, false, brushi);
            }

            
            if (encoder.sendFrame(&frame)) {
                VENC_STREAM_S stStream;
                memset(&stStream, 0, sizeof(stStream));
                session->writeFrame(stStream.pstPack->pu8Addr, stStream.pstPack->u32Len, stStream.pstPack->u64PTS);
                encoder.getStream(&stStream);
                 // Prepare RTSP data

                // Release stream back to encoder
                CVI_VENC_ReleaseStream(encoder.getVencChn(), &stStream);
            }
            decoder.releaseFrame(&frame);
        }

        CVI_TDL_Free(&obj);
        av_packet_unref(&pkt);
    }
    // CVI_RTSP_DestroySession(rtspCtx, session);
    // CVI_RTSP_Stop(rtspCtx);
    // CVI_RTSP_Destroy(&rtspCtx);
    reader.close();
    return 0;
}