set(PROJECT_NAME gvision)

set(SRCs

        AI/detection.cc
        AI/detection.h
        decoder/hw_decoder.cc
        decoder/hw_decoder.h
        rtsp_reader/rtsp_reader.cc
        rtsp_reader/rtsp_reader.h


)

add_library(${PROJECT_NAME} SHARED ${SRCs})

target_link_libraries(${PROJECT_NAME} PRIVATE
        ${OpenCV_LIBS}
        ${FFMPEG_LIBS}
        ${CVI_TDL_LIBS}
        )
