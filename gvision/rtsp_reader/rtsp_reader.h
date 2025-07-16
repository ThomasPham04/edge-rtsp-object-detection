// rtsp_reader.h
#pragma once
extern "C" {
    #include <libavformat/avformat.h>
}
#include <iostream>
#include <string>
class RtspReader {
public:
    bool open(const std::string& url);
    bool readPacket(AVPacket &pkt);
    void close();
    int getVideoStreamIndex() const { return videoStreamIndex; }
private:
    AVFormatContext *fmtCtx = avformat_alloc_context();
    int videoStreamIndex = -1;
};
