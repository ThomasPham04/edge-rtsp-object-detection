// rtsp_reader.cpp
#include "rtsp_reader.h"

RtspReader::~RtspReader() {
    close();
}

bool RtspReader::open(const std::string& url) {
    close();

    width = 0;
    height = 0;
    codecType.clear();
    videoStreamIndex = -1;

    avformat_network_init();
    std::cout << "Opening RTSP stream: " << url << "\n";
    AVDictionary *opts = nullptr;
    av_dict_set(&opts, "rw_timeout", "3000000", 0);
    av_dict_set(&opts, "stimeout", "3000000", 0);
    av_dict_set(&opts, "rtsp_transport", "tcp", 0);

    if (avformat_open_input(&fmtCtx, url.c_str(), nullptr, &opts) < 0) {
        av_dict_free(&opts);
        avformat_close_input(&fmtCtx);
        return false;
    } 

    if (avformat_find_stream_info(fmtCtx, nullptr) < 0) {
        av_dict_free(&opts);
        avformat_close_input(&fmtCtx);
        return false;
    }

    for (unsigned int i = 0; i < fmtCtx->nb_streams; ++i) {
        if (fmtCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoStreamIndex = i;
            break;
        }
    }

    if (videoStreamIndex < 0) {
        std::cerr << "No video stream found in RTSP input\n";
        av_dict_free(&opts);
        avformat_close_input(&fmtCtx);
        return false;
    }

    AVCodecParameters *codecPar = fmtCtx->streams[videoStreamIndex]->codecpar;

    switch (codecPar->codec_id) {
        case AV_CODEC_ID_H264:
            this->codecType = "H.264";
            break;
        case AV_CODEC_ID_HEVC:
            this->codecType = "H.265";
            break;
        default:
            std::cerr << "Unsupported video codec id: " << codecPar->codec_id << "\n";
            av_dict_free(&opts);
            avformat_close_input(&fmtCtx);
            return false;
    }

    this->width = codecPar->width;
    this->height = codecPar->height;
    if (this->width <= 0 || this->height <= 0) {
        std::cerr << "Invalid video dimensions: " << this->width << "x" << this->height << "\n";
        av_dict_free(&opts);
        avformat_close_input(&fmtCtx);
        return false;
    }

    av_dict_free(&opts);

    return true;
}

bool RtspReader::readPacket(AVPacket &pkt) {
    int maxRetry = 5;  
    int retry = 0;
    int ret;

    while (retry < maxRetry) {
        ret = av_read_frame(fmtCtx, &pkt);
        if (ret >= 0) {
            if (pkt.stream_index == videoStreamIndex) {
                // std::cout << "\nGot video packet\n";
                return true;
            }
            av_packet_unref(&pkt);
            continue;
        } else {
            if (ret == AVERROR(EAGAIN)) {
                std::cerr << "Retrying...\n";
                retry++;
                av_usleep(1000 * 100); 
                continue;
            }
            if (ret == AVERROR_EOF) {
                std::cerr << "End of RTSP stream\n";
                return false;
            }
            std::cerr << "Error reading frame: " << ret << "\n";
            return false;
        }
    }

    std::cerr << "Max retry exceeded, giving up\n";
    return false;
}


void RtspReader::close() {
    if (fmtCtx) avformat_close_input(&fmtCtx);
    avformat_network_deinit();
}

