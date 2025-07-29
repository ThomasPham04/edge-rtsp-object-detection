# Cần sửa thành đúng thư mục chứa OpenCVConfig.cmake
# set(OpenCV_DIR "/workspace/opencv/share/OpenCV")
# OpenCVConfig.cmake  opencv-config.cmake
set(OpenCV_DIR "/home/thuyen/opencv-mobile-4.10.0-licheerv-nano/lib/cmake/opencv4")
find_package(OpenCV REQUIRED)

message(STATUS "OpenCV Libs: \n${OpenCV_LIBS}\n")
message(STATUS "OpenCV Libraries: \n${OpenCV_LIBRARIES}\n")
message(STATUS "OpenCV Headers: \n${OpenCV_INCLUDE_DIRS}\n")

include_directories(${OpenCV_INCLUDE_DIRS})

