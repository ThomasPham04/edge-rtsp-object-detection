#include(FetchContent)
#
#FetchContent_Declare(
#        spdlog-cpp
#        GIT_REPOSITORY https://github.com/gabime/spdlog.git
#        GIT_TAG v1.13.0
#        GIT_SHALLOW TRUE
#)
#FetchContent_GetProperties(spdlog-cpp)
#
#if(NOT spdlog-cpp_POPULATED)
#    message(STATUS "Fetching https://github.com/gabime/spdlog.git ...")
#    FetchContent_Populate(spdlog-cpp)
#    add_subdirectory(${spdlog-cpp_SOURCE_DIR} ${spdlog-cpp_BINARY_DIR})
#endif()
find_package(spdlog REQUIRED)
message(spdlog_INCLUDE_DIRS ${spdlog_INCLUDE_DIRS})
message(spdlog_LIBRARIES ${spdlog_LIBRARIES})