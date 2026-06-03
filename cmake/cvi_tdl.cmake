get_filename_component(REPO_ROOT "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
get_filename_component(SDK_BASE "${REPO_ROOT}/.." ABSOLUTE)

set(MW_PATH $ENV{MW_PATH})
set(IVE_PATH $ENV{IVE_PATH})
set(TPU_PATH $ENV{TPU_PATH})
set(CVI_TDL_DIR $ENV{CVI_TDL_DIR})
set(SDK_VER $ENV{SDK_VER})
set(CHIP $ENV{CHIP})
set(USE_TPU_IVE $ENV{USE_TPU_IVE})

if (NOT MW_PATH)
    set(MW_PATH "${SDK_BASE}/middleware/v2")
endif()

if (NOT CVI_TDL_DIR)
    set(CVI_TDL_DIR "${SDK_BASE}/cvitek_tdl_sdk")
endif()

if (NOT IVE_PATH)
    set(IVE_PATH "${CVI_TDL_DIR}/sample/3rd/ive")
endif()

if (NOT TPU_PATH)
    set(TPU_PATH "${CVI_TDL_DIR}/sample/3rd/tpu")
endif()

if (NOT USE_TPU_IVE)
    set(USE_TPU_IVE "ON")
endif()

if (NOT CHIP)
    set(CHIP "CV181X")
endif()

if (NOT SDK_VER)
    set(SDK_VER "musl_riscv64")
endif()


if (SDK_VER STREQUAL "musl_riscv64")
    set(ARCH "riscv64")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=rv64imafd -D_MIDDLEWARE_V2_")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mcpu=c906fdv")
endif()
#############################################
# MIDDLEWARE 
#############################################

set(MW_LIB_PATH "${MW_PATH}/lib")
set(MW_INC_PATH "${MW_PATH}/include")
set(MW_PANEL_INC_PATH "${MW_PATH}/component/panel")
set(MW_SAMPLE_PATH "${MW_PATH}/sample/common")
set(MW_3RD_LIB_PATH "${MW_PATH}/lib/3rd")
message(STATUS "==>MWPATH: ${MW_PATH}")
message(STATUS "==>TPUPATH:${TPU_PATH}")
message(STATUS "==>IVEPATH:${IVE_PATH}")
set(MW_LIBS
    ini sns_full sample isp vdec venc awb ae af cvi_bin cvi_bin_isp misc isp_algo sys vpu 
)

set (MW_AUDIO_LIBS
    cvi_audio cvi_vqe cvi_ssp tinyalsa cvi_VoiceEngine cvi_RES1 cli
)

set (MW_ALL_LIBS
    ${MW_LIBS}
    ${MW_AUDIO_LIBS}
)
#############################################
# TDL SDK
#############################################
set(SDK_ROOT_PATH "${CVI_TDL_DIR}")

set(SDK_LIB_PATH "${SDK_ROOT_PATH}/lib")
set(SDK_INC_PATH "${SDK_ROOT_PATH}/include")
set(SDK_3RD_LIB_PATH "${SDK_ROOT_PATH}/sample/3rd/lib")
set(SDK_TDL_INC_PATH "${SDK_INC_PATH}/cvi_tdl")
set(SDK_APP_INC_PATH "${SDK_INC_PATH}/cvi_tdl_app")
set(SDK_SAMPLE_INC_PATH "${SDK_ROOT_PATH}/sample/3rd/include")
set(SDK_SAMPLE_UTILS_PATH "${SDK_ROOT_PATH}/sample/utils")

set(SDK_TDL_LIBS 
    cvi_tdl
)

set(SDK_APP_LIBS
    cvi_tdl_app    
)
#############################################
# OPENCV
#############################################
set(OPENCV_INC_PATH "${SDK_ROOT_PATH}/sample/3rd/opencv/include")
set(OPENCV_LIB_PATH "${SDK_ROOT_PATH}/sample/3rd/opencv/lib")

set(OPENCV_LIBS
    opencv_core
    opencv_imgproc
    opencv_imgcodecs
)
#############################################
# isp
#############################################

set(ISP_SDK_INC_PATH "${MW_PATH}/include/isp/cv181x/")
# set(ISP_SDK_LIB_PATH "${MW_PATH}/lib")

#############################################
# IVE
#############################################

set(IVE_SDK_INC_PATH "${IVE_PATH}/include")
set(IVE_SDK_LIB_PATH "${IVE_PATH}/lib")

set(IVE_LIBS
    cvi_ive_tpu
)


#############################################
# TPU
#############################################
set(TPU_LIB_PATH "${TPU_PATH}/lib")

set(TPU_LIBS
    cnpy
    cvikernel
    cvimath
    cviruntime
    z
    m
)

#############################################
# RTSP
#############################################
set(RTSP_INC_PATH "${SDK_ROOT_PATH}/sample/3rd/rtsp/include/cvi_rtsp")
set(RTSP_LIB_PATH "${SDK_ROOT_PATH}/sample/3rd/rtsp/lib")

set(RTSP_LIBS
    cvi_rtsp
)

#############################################
# STB
#############################################
set(STB_INC_PATH "${SDK_ROOT_PATH}/sample/3rd/stb/include")
#############################################
# CVI LIB 
#############################################
set(MD_INC_PATH "${SDK_ROOT_PATH}/include/cvi_md")
set(DRAW_RECT_INC_PATH "${SDK_ROOT_PATH}/include/cvi_draw_rect")
set(PREPROCESS_INC_PATH "${SDK_ROOT_PATH}/include/cvi_preprocess")

# #############################################
# # mmf_server 
# #############################################
# set(MMF_INC_PATH "${MW_PATH}/sample/test_mmf/media_server-1.0.1/include") 
# set (MMF_LIB_PATH "${MW_PATH}/sample/test_mmf/media_server-1.0.1/")

#############################################
# utils 
#############################################
# set(UTILS_INC_PATH  "${SDK_ROOT_PATH}/sample/utils")

set(CVI_TDL_INCLUDE_DIRS
    ${MW_INC_PATH}
    ${MW_INC_PATH}/linux
    ${MW_PANEL_INC_PATH}
    ${MW_SAMPLE_PATH}
    ${SDK_INC_PATH}
    ${SDK_TDL_INC_PATH}
    ${SDK_APP_INC_PATH}
    ${SDK_SAMPLE_INC_PATH}
    ${SDK_SAMPLE_UTILS_PATH}
    ${OPENCV_INC_PATH}
    ${IVE_SDK_INC_PATH}
    ${RTSP_INC_PATH}
    ${STB_INC_PATH}
    ${MD_INC_PATH}
    ${DRAW_RECT_INC_PATH}
    ${PREPROCESS_INC_PATH}
    ${MMF_INC_PATH}
    ${ISP_SDK_INC_PATH}
)

set(CVI_TDL_LIBRARY_DIRS
    ${MW_LIB_PATH}
    ${MW_3RD_LIB_PATH}
    ${SDK_LIB_PATH}
    ${SDK_3RD_LIB_PATH}
    ${OPENCV_LIB_PATH}
    ${IVE_SDK_LIB_PATH}
    ${TPU_LIB_PATH}
    ${RTSP_LIB_PATH}
    ${MMF_LIB_PATH}
)

set(CVI_TDL_LIBS
    ${MW_LIBS}
    ${MW_AUDIO_LIBS}
    ${SDK_TDL_LIBS}
    ${SDK_APP_LIBS}
    ${OPENCV_LIBS}
    ${IVE_LIBS}
    ${TPU_LIBS}
    ${RTSP_LIBS}
    pthread
    atomic
    cvi_draw_rect
)

message(STATUS "===========!!!!!!!!!!!!!!!!!!!1>${SDK_TDL_INC_PATH}")
message(STATUS "Path needed: ${MW_INC_PATH}")
message(STATUS "=========>${CVI_TDL_INCLUDE_DIRS}")
include_directories(
    ${CVI_TDL_INCLUDE_DIRS}
    ${MW_INC_PATH}/linux
    )
link_directories(${CVI_TDL_LIBRARY_DIRS})
