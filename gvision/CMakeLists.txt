set(PROJECT_NAME gvision)

set(SRCs

        sys/sys_init.cc
        sys/sys_init.h
        encoder/hw_encoder.cc
        encoder/hw_encoder.h
        AI/detector.cc
        AI/detector.h
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
        ${BYTETRACK_LIB}
        )
