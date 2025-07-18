#!/bin/bash
help() {
	echo "Choose your option 1 or 2"
	echo "Option 1: retain CmakeCache"
	echo "Option 2: remove CmakeCache"
}
a=-1
if [ $# -ne 1 ] 
then
	help
	exit 1
else
	a=$1
fi 

if [ $a -eq 1 ] 
then
	make clean

	make -j4

	echo "Done"
	exit 
elif [ $a -eq 2 ] 
then
	rm -rf CMakeCache.txt
	make clean
	cmake .. -DCMAKE_TOOLCHAIN_FILE=../cmake/riscv-toolchain.cmake

	make -j4

	echo "Done"
	exit
fi		
