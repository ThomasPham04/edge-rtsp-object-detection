// rtsp_reader.cpp
#include "rtsp_reader.h"
bool RtspReader::open(const std::string& url) {
    avformat_network_init();
    std::cout << "Opening RTSP stream: " << url << "\n";
    AVDictionary *opts = nullptr;
    av_dict_set(&opts, "rw_timeout", "3000000", 0);
    av_dict_set(&opts, "stimeout", "3000000", 0);
    av_dict_set(&opts, "rtsp_transport", "tcp", 0);
    if (avformat_open_input(&fmtCtx, url.c_str(), nullptr, &opts) <0){
        avformat_close_input(&fmtCtx);
        return false;
    } 
    if (avformat_find_stream_info(fmtCtx, nullptr) < 0) {
        avformat_close_input(&fmtCtx);
        return false;
    }
    for (unsigned int i = 0; i < fmtCtx->nb_streams; ++i) {
        if (fmtCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoStreamIndex = i;
            break;
        }
    }
    av_dict_free(&opts);
    return videoStreamIndex >= 0;
}

bool RtspReader::readPacket(AVPacket &pkt) {
    int ret = av_read_frame(fmtCtx, &pkt);
    if (ret < 0){
        std::cout << "Error reading frame: " << ret << "\n";
        return false;
    }
    
    if (pkt.stream_index == videoStreamIndex){
        std::cout << "Got video packet, size" << pkt.size << "\n";
        return true;
    }
    av_packet_unref(&pkt);
    return false;
}

void RtspReader::close() {
    std::cout << "close\n" << std::flush;
    if (fmtCtx) avformat_close_input(&fmtCtx);
    avformat_network_deinit();
}


