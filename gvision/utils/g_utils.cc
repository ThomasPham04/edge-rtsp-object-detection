//
// Created by ryan on 20/02/2024.
//

#include "g_utils.h"
#include <iomanip>
#include <iostream>
namespace gvision {
    std::string time_point_to_string(const std::chrono::system_clock::time_point &tp) {
        // Convert to time_t which represents the time in seconds since the epoch
        std::time_t time_t_format = std::chrono::system_clock::to_time_t(tp);

        // Convert to tm structure for local time
        std::tm *tm_format = std::localtime(&time_t_format);

        // Format the time into a string
        std::ostringstream oss;
        oss << std::put_time(tm_format, "%Y-%m-%d %H:%M:%S");
        return oss.str();
    }

    std::chrono::system_clock::time_point parseTime(const std::string &timeStr) {
        std::tm tm = {};
        std::istringstream ss(timeStr);
        ss >> std::get_time(&tm, "%H:%M:%S");
        if (ss.fail()) {
            throw std::invalid_argument("Invalid time format: " + timeStr);
        }
        time_t t = std::mktime(&tm);
        if (t == -1) {
            throw std::runtime_error("Failed to convert time string to time point");
        }
        return std::chrono::system_clock::from_time_t(t);
    }

    std::string generate_uuid_v4() {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_int_distribution<> dis(0, 15);
        static std::uniform_int_distribution<> dis2(8, 11);
        std::stringstream ss;
        int i;
        ss << std::hex;
        for (i = 0; i < 8; i++) {
            ss << dis(gen);
        }
        ss << "-";
        for (i = 0; i < 4; i++) {
            ss << dis(gen);
        }
        ss << "-4";
        for (i = 0; i < 3; i++) {
            ss << dis(gen);
        }
        ss << "-";
        ss << dis2(gen);
        for (i = 0; i < 3; i++) {
            ss << dis(gen);
        }
        ss << "-";
        for (i = 0; i < 12; i++) {
            ss << dis(gen);
        }
        return ss.str();
    }

    std::string getCurrentDateString() {
        // Get the current time point
        std::chrono::system_clock::time_point now = std::chrono::system_clock::now();

        // Convert the time point to a time_t object
        std::time_t currentTime = std::chrono::system_clock::to_time_t(now);

        // Convert the time_t to a local time structure
        std::tm *localTime = std::localtime(&currentTime);

        // Format the date as a string
        std::ostringstream oss;
        oss << std::put_time(localTime, "%Y-%m-%d");

        return oss.str();
    }

    std::string getAppearedAt(const std::string &format) {
        auto now = std::chrono::system_clock::now();
        std::time_t now_c = std::chrono::system_clock::to_time_t(now);
        // Convert current time to std::tm for formatting
        std::tm time_info;
        gmtime_r(&now_c, &time_info); // Use gmtime_r for thread safety
        // Create a std::stringstream to build the formatted date string
        std::stringstream ss;
        ss << std::put_time(&time_info, format.c_str());
        return ss.str();
    }
    std::string getCurrentUTCTimeISO8601() {
        // Get the current time
        auto now = std::chrono::system_clock::now();

        // Convert to time_t to get the calendar time
        std::time_t now_c = std::chrono::system_clock::to_time_t(now);

        // Convert to tm structure for UTC
        std::tm utc_tm;
        gmtime_r(&now_c, &utc_tm); // On Unix-like systems
        //gmtime_s(&utc_tm, &now_c); // On Windows

        // Format the time
        std::ostringstream oss;
        oss << std::put_time(&utc_tm, "%Y-%m-%dT%H:%M:%S");

        // Get the milliseconds
        auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

        // Add milliseconds to the formatted string
        oss << '.' << std::setw(3) << std::setfill('0') << milliseconds.count() << 'Z';

        return oss.str();
    }

    void printEnvironmentVariable() {
        std::cout << "start GSV AI \n";
        const char *value = std::getenv("GSV_AI_TAG");
        if (value) {
            std::cout << "GSV_AI_TAG: " << value << std::endl;
        } else {
            std::cout << "Environment variable GSV_AI_TAG " << " is not set." << std::endl;
        }
    }

}