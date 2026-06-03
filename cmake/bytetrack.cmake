get_filename_component(REPO_ROOT "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
get_filename_component(SDK_BASE "${REPO_ROOT}/.." ABSOLUTE)

if(DEFINED ENV{BYTETRACK_PATH})
    set(BYTETRACK_PATH "$ENV{BYTETRACK_PATH}")
else()
    set(BYTETRACK_PATH "${SDK_BASE}/ByteTrack-cpp")
endif()

set (BYTETRACK_INC_PATH "${BYTETRACK_PATH}/include")
set (BYTETRACK_LIB_PATH "${BYTETRACK_PATH}/build_riscv")

set(BYTETRACK_LIB
    bytetrack
)

include_directories(
    ${BYTETRACK_INC_PATH}
    ${BYTETRACK_INC_PATH}/ByteTrack
)

link_directories(
    ${BYTETRACK_LIB_PATH}
)
