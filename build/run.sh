#!/bin/bash

set -e
#Xoa cache
rm -rf CMakeCache.txt

cmake .. -DCMAKE_TOOLCHAIN_FILE=../cmake/riscv-toolchain.cmake

make -j4

echo "Done"
