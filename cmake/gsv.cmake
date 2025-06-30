set(BUILD_ALL OFF)

set(APP_LIBS
        ${OpenCV_LIBS}
        ${CURL_LIBRARIES}
        ${FFMPEG_LIBS}
        spdlog::spdlog
        ${YAML_CPP_LIBRARIES}

        )


set(GSV_ABSTRACT_LIB g_abstract)
set(GSV_COMMON_LIB g_common)
set(GSV_UTILS_LIB g_utils)
set(GSV_STREAMING_LIB g_streaming)
set(GSV_VAULT_LIB g_vault)
set(GSV_NOTIFICATION_LIB g_notification)
set(GSV_VISION_LIB g_vision)

set(GSV_CAMERA_LPR_LIB g_camera_lpr)
set(GSV_CAMERA_FACE_LIB g_camera_face)
set(GSV_CAMERA_HUMAN_LIB g_camera_human)


set(GSV_COMMON_LIBS
        ${GSV_ABSTRACT_LIB}
        ${GSV_COMMON_LIB}
        ${GSV_UTILS_LIB}
        ${GSV_STREAMING_LIB}
        ${GSV_NOTIFICATION_LIB}
        ${GSV_VAULT_LIB}
        ${GSV_VISION_LIB}
        )

SET(GSV_LPR_LIBS
        ${GSV_COMMON_LIBS}
        ${GSV_CAMERA_LPR_LIB}
        )

if (BUILD_APP_TV)
    set(BUILD_LPR ON)
endif ()

if (BUILD_ALL)
    set(BUILD_LPR ON)
    set(BUILD_FACE ON)
    set(BUILD_HUMAN ON)

endif ()