//
// Created by CuongTV on 26/12/2024.
// Copyright (c) GreenStreamVision
//
#include <opencv2/opencv.hpp>
#include <functional>
#ifndef G_VISION_Callback_H
#define G_VISION_Callback_H

namespace gvision {
 typedef std::function<void(cv::Mat &frame)> NewFrameCallback;

}

#endif //G_VISION_Callback_H