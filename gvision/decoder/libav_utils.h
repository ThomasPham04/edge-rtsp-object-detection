//
// Created by CuongTV on 18/01/2025.
// Copyright (c) GreenStreamVision
//

#ifndef G_VISION_libav_utils_H
#define G_VISION_libav_utils_H
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
 bool isKeyframe(AVPacket *pkt);
 
 bool avFrame2cvMat(AVFrame *frame, cv::Mat &dest);
 
 long int detectMotion(AVFrame *av_frame, int blockThreshold = 25, int magnitudeThreshold = 10);
	
}

#endif //G_VISION_libav_utils_H