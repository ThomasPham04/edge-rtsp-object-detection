set(PROJECT_NAME gvision)

set(SRCs
        callback/Callback.h

        models/ObjectDetection.cc
        models/NotificationMessage.cc

        config/AppConfig.cc
        config/StreamConfig.cc
        config/HumanDetectionConfig.cc
        config/StreamList.cc

        decoder/DecoderAbstract.cc
        decoder/DecoderAbstract.h
        decoder/LibavDecoder.cc
        decoder/LibavDecoder.h
        decoder/LibavDecodeContext.cc
        decoder/libav_utils.cc
        decoder/hw_decoder.cc
        decoder/hw_decoder.h

        vision/HumanProcessor.cc
        camera/HumanCamera.cc
        camera/HdCamera.cc
        models/camera_status.h
        utils/utils.cc
        utils/utils.h
        utils/fs_utils.cpp
        utils/cv_utils.cpp
        utils/g_utils.cpp

)


add_library(${PROJECT_NAME} SHARED ${SRCs})
target_link_libraries(${PROJECT_NAME} PRIVATE
        ${OpenCV_LIBS}
        ${YAML_CPP_LIBRARIES}
        ${FFMPEG_LIBS}
        ${CVI_MPI_LIBS}
        nlohmann_json::nlohmann_json
        )
