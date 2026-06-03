# Cần sửa thành đúng thư mục chứa OpenCVConfig.cmake
# set(OpenCV_DIR "/workspace/opencv/share/OpenCV")
# OpenCVConfig.cmake  opencv-config.cmake
get_filename_component(REPO_ROOT "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
get_filename_component(SDK_BASE "${REPO_ROOT}/.." ABSOLUTE)

if(DEFINED ENV{OpenCV_DIR})
    set(OpenCV_DIR "$ENV{OpenCV_DIR}")
else()
    set(OpenCV_DIR "${SDK_BASE}/opencv-mobile-4.10.0-licheerv-nano/lib/cmake/opencv4")
endif()

find_package(OpenCV REQUIRED)

message(STATUS "OpenCV Libs: \n${OpenCV_LIBS}\n")
message(STATUS "OpenCV Libraries: \n${OpenCV_LIBRARIES}\n")
message(STATUS "OpenCV Headers: \n${OpenCV_INCLUDE_DIRS}\n")

include_directories(${OpenCV_INCLUDE_DIRS})
