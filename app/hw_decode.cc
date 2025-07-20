#include "rtsp_reader/rtsp_reader.h"
#include "decoder/hw_decoder.h"
#include "AI/detection.h"
#include <opencv2/opencv.hpp>
#include <unistd.h>
int main() {
    std::string modelBasePath = "/home/thuyen/model";
    RtspReader reader;
    HardwareDecoder decoder;
    AIDetection sample;
    std::cout << "-------------------Hardware Decoder-------------------\n";
    if (!reader.open("rtsp://admin:gsv@101Aa@vtech.greenstreamvision.com:8557/Streaming/channels/101")){
        std::cout << "Failed to open\n";
        return -1;
    }  else {
        std::cout << "Open successfully\n";
    }
    decoder.init(2560, 1440);
    int frame_count = 0;
    AVPacket pkt;
    while (reader.readPacket(pkt)) {
        std::cout   << "Packet size: " << pkt.size
                    << " PTS: " << pkt.pts
                    << " Stream index: " << pkt.stream_index << "\n";
        if (!decoder.sendPacket(pkt.data, pkt.size, pkt.pts)){
            std::cerr << "Failed to send packet to decoder\n";
            av_packet_unref(&pkt);
            continue;
        }

        VIDEO_FRAME_INFO_S frame;
        CVI_TDL_SUPPORTED_MODEL_E model = CVI_TDL_SUPPORTED_MODEL_YOLOV8_DETECTION;
        cvtdl_object_t obj;
        if (decoder.getFrame(&frame)) {
            sample.objDectection(modelBasePath + "yolov8n.cvimodel", &frame, model, &obj);
            std::cout   << "obj infor: " << obj.info << std::endl 
                        << "obj width: " << obj.width << std::endl
                        << "obj height" << obj.height << std::endl;
            decoder.releaseFrame(&frame);
        } else {
            std::cout << "Can't get any frame" << std::endl;
        }
        av_packet_unref(&pkt);
        frame_count++;
    }

    decoder.cleanup();
    reader.close();
    return 0;
}
