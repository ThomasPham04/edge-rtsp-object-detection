PROJECT(app_decode C CXX)

add_executable(${PROJECT_NAME}_rtsp rtsp_stream.cc)
message(STATUS "FFMPEG_LIBS: \n${FFMPEG_LIBS}\n")
target_link_libraries(${PROJECT_NAME}_rtsp
        gvision

        ${OpenCV_LIBS}
        ${FFMPEG_LIBS}
        )
