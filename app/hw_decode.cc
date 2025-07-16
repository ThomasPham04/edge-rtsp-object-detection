#include "../gvision/rtsp_reader/rtsp_reader.h"
#include "../gvision/decoder/hw_decoder.h"
#include <opencv2/opencv.hpp>
#include <unistd.h>
int main() {
    RtspReader reader;
    HardwareDecoder decoder;
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
        if (decoder.getFrame(&frame)) {
            std::cout << "Sucessfully decoded frame\n";
            // cv::Mat yuv(frame.stVFrame.u32Height * 3 / 2, frame.stVFrame.u32Width, CV_8UC1, frame.stVFrame.pu8VirAddr[0]);
            // cv::Mat bgr;
            // cv::cvtColor(yuv, bgr, cv::COLOR_YUV2BGR_I420);
            std::cout << "\n-------------------INFOR-------------------\n";
            std::cout   <<"Height: " << frame.stVFrame.u32Height << " " << "Width: " << frame.stVFrame.u32Width << " "
                        <<"BayerFormat: " << frame.stVFrame.enBayerFormat << " " <<"enCompressMode: " << frame.stVFrame.enCompressMode << " "
                        <<"enColorGamut: " << frame.stVFrame.enColorGamut << " " <<"enVideoFormat: " << frame.stVFrame.enVideoFormat << "\n";
            std::cout << "\n-------------------INFOR-------------------\n";

            // if (bgr.empty()) {
            //     std::cerr << "BGR Mat is empty\n";
            // } else {
            //     std::cout << "BGR Mat is not empty\n";
            //     // cv::imshow("frame", bgr); 
            //     cv::imwrite("hw_frame.jpg", bgr);
            // }
            // cv::waitKey(1);
            decoder.releaseFrame(&frame);
            // std::cout << "hw_decode: " << bgr.cols << "\n";
        }
        av_packet_unref(&pkt);
        frame_count++;
    }

    decoder.cleanup();
    reader.close();
    return 0;
}
