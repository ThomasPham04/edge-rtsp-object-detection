//
// Created by ryan on 20/02/2024.
//

#ifndef GSV_HUMAN_G_UTILS_H
#define GSV_HUMAN_G_UTILS_H

#include <string>
#include <random>
#include <sstream>
#include <cstdlib>
#include <chrono>
#include <ctime>
#include <iomanip>  // for std::put_time

namespace gvision {
    std::chrono::system_clock::time_point parseTime(const std::string &timeStr);

    std::string time_point_to_string(const std::chrono::system_clock::time_point &tp);

    std::string generate_uuid_v4();

    std::string getCurrentDateString();

    std::string getAppearedAt(const std::string &format);
    std::string getCurrentUTCTimeISO8601();

    void printEnvironmentVariable();

}

#endif //GSV_HUMAN_G_UTILS_H
