#!/bin/sh
rm build/CMakeCache.txt
mkdir build
cd build
cmake ..
echo "Created build directory, make sure you are in the build directory and run make to compile project!"
