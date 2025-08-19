PROJECT(app_AI_decode C CXX)

# Add compiler flags to suppress OpenCV warnings
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-error=class-memaccess -Wno-error=deprecated-copy")

add_executable(${PROJECT_NAME} main.cc ../gvision/AI/detector.cc ../gvision/decoder/hw_decoder.cc ../gvision/rtsp_reader/rtsp_reader.cc ../gvision/sys/sys_init.cc ../gvision/encoder/hw_encoder.cc)
message(STATUS "FFMPEG_LIBS: \n${FFMPEG_LIBS}\n")
message(STATUS "CVI_TDL_SDK_LIBS: \n${CVI_TDL_LIBS}\n")
target_link_libraries(${PROJECT_NAME}
        gvision 
        bytetrack
        ${FFMPEG_LIBS}
        ${CVI_TDL_LIBS}
        ${BYTETRACK_LIB}
        )
