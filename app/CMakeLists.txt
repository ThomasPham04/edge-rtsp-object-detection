PROJECT(app_decode C CXX)
add_executable(${PROJECT_NAME}_rtsp main.cc)
message(STATUS "FFMPEG_LIBS: \n${FFMPEG_LIBS}\n")
message(STATUS "CVI_TDL_SDK_LIBS: \n${CVI_TDL_LIBS}\n")
target_link_libraries(${PROJECT_NAME}_rtsp
        gvision 
        ${FFMPEG_LIBS}
        ${CVI_TDL_LIBS}
        )
