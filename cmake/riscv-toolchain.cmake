# riscv-toolchain.cmake
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR riscv64)

get_filename_component(REPO_ROOT "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
get_filename_component(SDK_BASE "${REPO_ROOT}/.." ABSOLUTE)

if(DEFINED ENV{TOOLCHAIN_DIR})
    set(TOOLCHAIN_DIR "$ENV{TOOLCHAIN_DIR}")
else()
    set(TOOLCHAIN_DIR "${SDK_BASE}/host-tools/gcc/riscv64-linux-musl-x86_64")
endif()

set(CMAKE_C_COMPILER ${TOOLCHAIN_DIR}/bin/riscv64-unknown-linux-musl-gcc)
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_DIR}/bin/riscv64-unknown-linux-musl-g++)
set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_FIND_ROOT_PATH ${TOOLCHAIN_DIR}/riscv64-unknown-linux-musl)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
