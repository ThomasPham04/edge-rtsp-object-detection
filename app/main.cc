#include "rtsp_reader/rtsp_reader.h"
#include "decoder/hw_decoder.h"
#include "AI/detector.h"
#include "sys/sys_init.h"
#include <csignal>
#include <unistd.h>
#include <atomic>
#include "BYTETracker.h"
// #include <opencv2/opencv.hpp>
//      rtsp://admin:gsv@101Aa@192.168.50.4:554/Streaming/channels/101
//      rtsp://admin:gsv@101Aa@192.168.50.5:554/Streaming/channels/101
//      rtsp://admin:admin@116.193.72.50:8554:8554/ch1/1
//      rtsp://admin:gsv@101Aa@vtech.greenstreamvision.com:8556/Streaming/channels/101
//      rtsp://admin:gsv@101Aa@vtech.greenstreamvision.com:8557/Streaming/channels/101

// bytetrack c++ + people counting
std::atomic<bool> stop(false);  

void int_handler(int signal){
    stop = true;
}
int main(int argc, char* argv[]) {
    RtspReader reader;
    signal(SIGINT, int_handler);
    std::cout << "-------------------Hardware Decoder-------------------\n";
    if (!reader.open("rtsp://admin:gsv@101Aa@vtech.greenstreamvision.com:8557/Streaming/channels/101")){
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
    HardwareDecoder decoder(srcWidth, srcHeight, codecType);
    AIDetection detector(srcWidth, srcHeight);
    AVPacket pkt;
    VIDEO_FRAME_INFO_S frame;
    CVI_TDL_SUPPORTED_MODEL_E model = CVI_TDL_SUPPORTED_MODEL_YOLOV8_DETECTION;
    cvtdl_object_t obj = {0}; 
    int count = 0;
    
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
            std::cout << "objnum:" << obj.size << std::endl;
            std::stringstream ss;
            ss << "boxes=[";
            for (uint32_t i = 0; i < obj.size; i++) {
                ss << "[" << obj.info[i].bbox.x1 << "," << obj.info[i].bbox.y1 << ","
                << obj.info[i].bbox.x2 << "," << obj.info[i].bbox.y2 << ","
                << obj.info[i].classes << "," << obj.info[i].bbox.score << "],";
            }
            ss << "]\n";
            std::cout << ss.str();
            decoder.releaseFrame(&frame);
        } else {
            std::cout << "Can't get any frame" << std::endl;
        }
        count++;
        CVI_TDL_Free(&obj);
        av_packet_unref(&pkt);
    }
    
    reader.close();
    return 0;
}
