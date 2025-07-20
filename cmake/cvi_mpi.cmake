# set(CVI_MW_DIR "/workspace/cvi_mpi")
add_definitions(-D__CV181X__)
set(CVI_MW_DIR "${CMAKE_CURRENT_SOURCE_DIR}/cvi_mpi")
find_package(PkgConfig REQUIRED)
set(ENV{PKG_CONFIG_PATH} "${CVI_MW_DIR}/pkgconfig:$ENV{PKG_CONFIG_PATH}")

pkg_check_modules(CVI_VDEC REQUIRED cvi_vdec)

include_directories(
  ${CVI_VDEC_INCLUDE_DIRS}

  ${CVI_MW_DIR}/include/linux/
)

set(CVI_MPI_LIBRARY_DIRS
    ${CVI_VDEC_LIBRARY_DIRS}
    ${CVI_MW_DIR}/lib
    ${CVI_MW_DIR}/lib/3rd
)

link_directories(${CVI_MPI_LIBRARY_DIRS})
message(STATUS "VDEC LIB DIRS: ${CVI_VDEC_LIBRARY_DIRS}")

set(CVI_MPI_LIBS
    ${CVI_VDEC_LIBRARIES}
    ${CVI_COMMON_LIBRARIES}
    ${CVI_MISC_LIBRARIES}
    ${CVI_AUDIO_LIBRARIES}
    ${CVI_IVE_LIBRARIES}
)

message(STATUS "CVI MW libs: ${CVI_MPI_LIBS}")
