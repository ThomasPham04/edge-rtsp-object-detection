get_filename_component(REPO_ROOT "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
get_filename_component(SDK_BASE "${REPO_ROOT}/.." ABSOLUTE)

if(DEFINED ENV{FFMPEG_ROOT})
    set(FFMPEG_ROOT "$ENV{FFMPEG_ROOT}")
else()
    set(FFMPEG_ROOT "${SDK_BASE}/ffmpeg_libs_n4.4.4.2")
endif()

include_directories(
    ${FFMPEG_ROOT}/include
)
link_directories(
    ${FFMPEG_ROOT}/lib
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
