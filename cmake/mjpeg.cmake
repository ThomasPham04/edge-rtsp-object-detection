include(FetchContent)

FetchContent_Declare(
        cpp-mjpeg-streamer
        GIT_REPOSITORY https://github.com/nadjieb/cpp-mjpeg-streamer.git
        GIT_TAG master # Can be a tag (yaml-cpp-x.x.x), a commit hash, or a branch name (master)
)
FetchContent_GetProperties(cpp-mjpeg-streamer)

if(NOT cpp-mjpeg-streamer_POPULATED)
    message(STATUS "Fetching cpp-mjpeg-streamer...")
    FetchContent_Populate(cpp-mjpeg-streamer)
    add_subdirectory(${cpp-mjpeg-streamer_SOURCE_DIR} ${cpp-mjpeg-streamer_BINARY_DIR})
endif()
