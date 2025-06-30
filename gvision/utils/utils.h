//
// Created by Cuong Truong on 19/01/2025.
//

#ifndef GSBOX_LIBS_UTILS_H
#define GSBOX_LIBS_UTILS_H
#include <iostream>
#include <iomanip>
#include <sstream>
#include <chrono>
#include <ctime>
#include <models/camera_status.h>
namespace gvision {
    std::string getUtcTimestampString();
    std::string CameraStatusToString(CameraStatus status);
}
#endif //GSBOX_LIBS_UTILS_H
