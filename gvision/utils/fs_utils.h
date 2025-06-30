//
// Created by ryan on 01/02/2024.
//

#ifndef GSV_HUMAN_FS_UTILS_H
#define GSV_HUMAN_FS_UTILS_H

#include <string>
#include <iostream>
#include <chrono>
#include <ctime>
#include <iomanip>  // for std::put_time

namespace gvision {

    bool createDirectory(const std::string &path);

    bool createDirectories(const std::string &path);

    bool moveFile(const char *srcFile, const char *destFile);

    bool deleteFile(const std::string &path);

    bool deleteFolder(std::string dirPath);

    bool ffmpegTs2Mp4(const std::string &inputFile, const std::string &outputFile);

//    [[maybe_unused]] bool mergeH264Files1(std::vector<std::string> filePaths, const std::string& outputFile);

    bool isFileExits(const std::string &filePath);
    bool isFolderExits(const std::string &path);

    void deleteOldFiles(const std::string& directoryPath, int thresholdSeconds);

}
#endif //GSV_HUMAN_FS_UTILS_H
