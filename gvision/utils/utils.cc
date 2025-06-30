//
// Created by Cuong Truong on 19/01/2025.
//

#include "utils.h"

namespace gvision {
    std::string getUtcTimestampString() {
        // Get the current time
        auto now = std::chrono::system_clock::now();
        std::time_t currentTime = std::chrono::system_clock::to_time_t(now);

        // Convert to UTC time
        std::tm utcTime;
#ifdef _WIN32
        gmtime_s(&utcTime, &currentTime); // Windows-specific function
#else
        gmtime_r(&currentTime, &utcTime); // POSIX-compliant function
#endif

        // Format the time into a string
        std::ostringstream timestamp;
        timestamp << std::put_time(&utcTime, "%Y-%m-%dT%H:%M:%SZ");

        return timestamp.str();
    }

    std::string CameraStatusToString(CameraStatus status) {
        switch (status) {
            case CameraStatus::LIVE: return "live";
            case CameraStatus::RETRY: return "retry";
            case CameraStatus::ERROR: return "error";
            case CameraStatus::STOP: return "stop";
            default: return "Unknown";
        }
    }

}