//
// Created by ryan on 01/02/2024.
//

#include "fs_utils.h"
#include <iostream>
#include <filesystem>
#include <iostream>
#include <filesystem>
#include <chrono>
#include <vector>
#include <fstream>


namespace fs = std::filesystem;

namespace gvision {
    bool isFileExits(const std::string &filePath) {
        return fs::exists(filePath);
    }

    bool isFolderExits(const std::string &path) {
        if (fs::exists(path) && fs::is_directory(path)) {
            return true;
        } else {
            return false;
        }
    }

    bool createDirectories(const std::string &directoryPath) {
        try {
            // Create the directory along with its parent directories
            if (!fs::exists(directoryPath)) {
                fs::create_directories(directoryPath);
                std::cout << "Directory created successfully: " << directoryPath << std::endl;
            }
            return true;
        } catch (const std::filesystem::filesystem_error &ex) {
            std::cerr << "Error: " << ex.what() << std::endl;
            return false;
        }

        return true;
    }

    bool createDirectory(const std::string &directoryPath) {
        try {
            // Check if the directory already exists
            if (!fs::exists(directoryPath)) {
                // Create the directory
                if (fs::create_directory(directoryPath)) {
                    std::cout << "Directory created successfully: " << directoryPath << std::endl;
                    return true;
                } else {
                    std::cerr << "Failed to create directory: " << directoryPath << std::endl;
                    return true;
                }
            } else {
                std::cout << "Directory already exists: " << directoryPath << std::endl;
                return true;
            }
        } catch (const std::filesystem::filesystem_error &ex) {
            std::cerr << "Error: " << ex.what() << std::endl;
            return true;
        }
        return false;

    }

    bool moveFile(const char *srcFile, const char *destFile) {
        if (std::rename(srcFile, destFile) != 0) {
            std::cerr << "Error: Failed to move file" << std::endl;
            return true;
        }

        std::cout << "moved " << srcFile << "to " << destFile << std::endl;
        return false;
    }

    bool deleteFile(const std::string &path) {
        // Attempt to delete the file
        if (std::remove(path.c_str()) != 0) {
            std::cerr << "Error deleting file: " << path << std::endl;
            return false;
        } else {
            std::cout << "File deleted successfully: " << path << std::endl;
            return true;
        }
    }

    bool deleteFolder(std::string dirPath) {
        std::error_code ec;
        fs::remove_all(dirPath, ec);

        if (ec) {
            std::cerr << "Error deleting directory: " << ec.message() << std::endl;
            return false;
        } else {
            std::cout << "Directory deleted successfully" << std::endl;
            return true;
        }
    }

    bool ffmpegTs2Mp4(const std::string &inputFile, const std::string &outputFile) {
        std::string ffmpegCommand = "ffmpeg -i " + inputFile + " -c copy " + outputFile;

        // Execute the ffmpeg command using std::system
        int result = std::system(ffmpegCommand.c_str());

        // Check the result of the ffmpeg command execution
        if (result == 0) {
            std::cout << "Conversion successful!" << std::endl;
            return true;
        } else {
            std::cerr << "Conversion failed with error code: " << result << std::endl;
            return false;
        }
    }

    void deleteOldFiles(const std::string &directoryPath, int thresholdSeconds) {
        try {
            auto now = std::chrono::system_clock::now();

            for (const auto &entry: fs::directory_iterator(directoryPath)) {
                if (fs::is_regular_file(entry)) {
                    auto fileTime = fs::last_write_time(entry);

                    // Convert `file_time_type` to `system_clock::time_point`
                    auto fileTimeConverted = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
                            fileTime - decltype(fileTime)::clock::now() + std::chrono::system_clock::now());

                    auto fileAge = std::chrono::duration_cast<std::chrono::seconds>(now - fileTimeConverted).count();

                    if (fileAge > thresholdSeconds) {
                        std::cout << "Deleting: " << entry.path() << " (Age: " << fileAge << " seconds)" << std::endl;
                        fs::remove(entry);
                    }
                }
            }
        } catch (const std::exception &e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }
//    bool mergeH264Files1(std::vector<std::string> filePaths, const std::string& outputFile) {
//
//        const std::string fileListPath = "/tmp/video.txt";
//
//        // Create the file list
//        std::ofstream fileList(fileListPath);
//        if (fileList.is_open()) {
//            for (auto path:filePaths) {
//                fileList << path << "\n";
//            }
//            fileList.close();
//            std::string command = "ffmpeg -f concat -safe 0 -i " + fileListPath + " -c copy " + outputFile;
//            int retCode = std::system(command.c_str());
//            return retCode ==0;
//
//        } else {
//            std::cerr << "Unable to create file list" << std::endl;
//            return false;
//        }
//
//    }
}