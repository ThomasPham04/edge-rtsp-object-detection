set(FFMPEG_ROOT /home/thuyen/ffmpeg_libs_n4.4.4.2)
include_directories(
    ${FFMPEG_ROOT}/include
)
link_directories(
    ${FFMPEG_ROOT}/lib
    ${CMAKE_CURRENT_SOURCE_DIR}/cvitek_tpu_sdk/lib
)

set(FFMPEG_LIBS
    avformat
    avcodec
    avutil
    pthread
    atomic
    swresample
)

message(STATUS "======>${FFMPEG_LIBS}")