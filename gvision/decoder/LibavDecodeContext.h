//
// Created by CuongTV on 18/01/2025.
// Copyright (c) GreenStreamVision
//

#ifndef G_VISION_LibavDecodeContext_H
#define G_VISION_LibavDecodeContext_H

#include <string>
#include <opencv2/opencv.hpp>
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavformat/avio.h>
#include <libswscale/swscale.h>
#include <libavutil/opt.h>
#include <libavutil/motion_vector.h>
#include <unistd.h>
}

namespace gvision {
    class LibavDecodeContext {
    public:
        LibavDecodeContext(const std::string uri, bool enableDetectMotion = false);

        ~LibavDecodeContext();

        bool init();

        AVFormatContext *pFormatCtx;

        AVCodecContext *pCodecCtx;

        bool isVideoPkt(AVPacket *packet);

        int fps{-1};
        std::string mUri;
        int height{0};
        int width{0};
        std::string codecName{"NA"};
        int videoStreamIndex{-1};

        int sendPkt(AVPacket *p_packet);

        bool decodeAvFrame(AVFrame *p_frame);
        cv::Mat avFrame2cvMat(AVFrame *frame);
    private:

        SwsContext *conversion;
        bool mEnableMotionDetection;

    };
}

#endif //G_VISION_LibavDecodeContext_H
