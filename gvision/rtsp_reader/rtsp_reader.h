// rtsp_reader.h
#pragma once
extern "C" {
    #include <libavformat/avformat.h>
}
#include <iostream>
#include <string>
class RtspReader {
private:
    int width = 0;
    int height = 0;
    std::string codecType;
    AVFormatContext *fmtCtx = avformat_alloc_context();
    int videoStreamIndex = -1;
public:
    bool open(const std::string& url);
    bool readPacket(AVPacket &pkt);
    void close();

    int getVideoStreamIndex() const { return videoStreamIndex; }
    int getVideoWidth() const {return width;}
    int getVideoHeight() const {return height;}
    std::string getCodecType() const {return codecType;}
};
