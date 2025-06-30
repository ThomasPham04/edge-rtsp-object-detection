# Gán root path
set(FFMPEG_DIR "/workspace/ffmpeg_libs_n4.4.4.2")

# Tìm pkg-config
find_package(PkgConfig REQUIRED)
set(ENV{PKG_CONFIG_PATH} "${FFMPEG_DIR}/lib/pkgconfig:$ENV{PKG_CONFIG_PATH}")

# Tìm FFmpeg qua pkg-config, bao gồm libswresample
pkg_check_modules(FFMPEG REQUIRED libavcodec libavformat libavutil libswscale libswresample)

# Kiểm tra xem FFmpeg có được tìm thấy không
if(NOT FFMPEG_FOUND)
    message(FATAL_ERROR "FFmpeg not found via pkg-config. Please check the FFmpeg installation and PKG_CONFIG_PATH.")
endif()

# Thêm thư mục include và thư viện
include_directories(${FFMPEG_INCLUDE_DIRS})
link_directories(${FFMPEG_LIBRARY_DIRS})

# Gán các thư viện FFmpeg vào biến FFMPEG_LIBS
set(FFMPEG_LIBS ${FFMPEG_LIBRARIES})

# In thông tin để kiểm tra
message(STATUS "FFmpeg include directories: ${FFMPEG_INCLUDE_DIRS}")
message(STATUS "FFmpeg libraries: ${FFMPEG_LIBS}")