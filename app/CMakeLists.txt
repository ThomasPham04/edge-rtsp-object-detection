PROJECT(app_decode C CXX)
# hw_decode.cc
add_executable(${PROJECT_NAME}_rtsp hw_decode.cc)
message(STATUS "FFMPEG_LIBS: \n${FFMPEG_LIBS}\n")
message(STATUS "CVI_MPI_LIBS: \n${CVI_MPI_LIBS}\n")
message(STATUS "CVI_TDL_SDK_LIBS: \n${CVI_TDL_SDK_LIBS}\n")
target_link_libraries(${PROJECT_NAME}_rtsp
        gvision 
        ${CVI_MPI_LIBS}
        ${OpenCV_LIBS}
        ${FFMPEG_LIBS}
        ${CVI_TDL_SDK_LIB}
        sbc
        # cvi_dnvqe
        )
