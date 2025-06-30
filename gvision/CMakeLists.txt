set(PROJECT_NAME gvision)

set(SRCs


        decoder/DecoderAbstract.cc
        decoder/DecoderAbstract.h
        decoder/LibavDecoder.cc
        decoder/LibavDecoder.h
        decoder/LibavDecodeContext.cc
        decoder/libav_utils.cc



)


add_library(${PROJECT_NAME} SHARED ${SRCs})
target_link_libraries(${PROJECT_NAME} PRIVATE
        ${OpenCV_LIBS}

        ${FFMPEG_LIBS}

        )
