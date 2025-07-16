# set(CVI_MW_DIR "/workspace/cvi_mpi")
add_definitions(-D__CV181X__)
set(CVI_MW_DIR "${CMAKE_CURRENT_SOURCE_DIR}/cvi_mpi")
find_package(PkgConfig REQUIRED)
set(ENV{PKG_CONFIG_PATH} "${CVI_MW_DIR}/pkgconfig:$ENV{PKG_CONFIG_PATH}")

# Load tất cả các module .pc bạn có
pkg_check_modules(CVI_VDEC REQUIRED cvi_vdec)
# pkg_check_modules(CVI_COMMON REQUIRED cvi_common)
# pkg_check_modules(CVI_MISC REQUIRED cvi_misc)
# pkg_check_modules(CVI_AUDIO REQUIRED cvi_audio)
# pkg_check_modules(CVI_IVE REQUIRED cvi_ive)

# Include và link dir chung
include_directories(
  ${CVI_VDEC_INCLUDE_DIRS}
#   ${CVI_COMMON_INCLUDE_DIRS}
#   ${CVI_MISC_INCLUDE_DIRS}
#   ${CVI_AUDIO_INCLUDE_DIRS}
#   ${CVI_IVE_INCLUDE_DIRS}
  ${CVI_MW_DIR}/include/linux/
)
# include_directories(${CVI_VDEC_INCLUDE_DIRS})

set(CVI_MPI_LIBRARY_DIRS
    ${CVI_VDEC_LIBRARY_DIRS}
    # ${CVI_COMMON_LIBRARY_DIRS}
    # ${CVI_MISC_LIBRARY_DIRS}
    # ${CVI_AUDIO_LIBRARY_DIRS}
    # ${CVI_IVE_LIBRARY_DIRS}
    ${CVI_MW_DIR}/lib
    ${CVI_MW_DIR}/lib/3rd
)

# Sử dụng để link thư viện
link_directories(${CVI_MPI_LIBRARY_DIRS})
message(STATUS "VDEC LIB DIRS: ${CVI_VDEC_LIBRARY_DIRS}")
# Gộp tất cả LIBS vào 1 biến
set(CVI_MPI_LIBS
    ${CVI_VDEC_LIBRARIES}
    ${CVI_COMMON_LIBRARIES}
    ${CVI_MISC_LIBRARIES}
    ${CVI_AUDIO_LIBRARIES}
    ${CVI_IVE_LIBRARIES}
)

message(STATUS "CVI MW libs: ${CVI_MPI_LIBS}")
