set(PROJECT_NAME gvision)

set(SRCs


        # decoder/DecoderAbstract.cc
        # decoder/DecoderAbstract.h
        # decoder/LibavDecoder.cc
        # decoder/LibavDecoder.h
        # decoder/LibavDecodeContext.cc
        # decoder/libav_utils.cc
        decoder/hw_decoder.cc
        decoder/hw_decoder.h
        rtsp_reader/rtsp_reader.cc
        rtsp_reader/rtsp_reader.h


)


add_library(${PROJECT_NAME} SHARED ${SRCs})
target_link_libraries(${PROJECT_NAME} PRIVATE
        ${OpenCV_LIBS}
        ${CVI_MPI_LIBS}
        ${FFMPEG_LIBS}
        sbc
        # cvi_dnvqe
        )
