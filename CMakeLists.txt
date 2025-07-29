cmake_minimum_required(VERSION 3.20)
project(rtsp_decoder C CXX)

include(cmake/ffmpeg.cmake)
include(cmake/cvi_tdl.cmake)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/gvision)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/cvitek-tdl-sdk/include)


add_subdirectory(gvision) 
add_subdirectory(app)
