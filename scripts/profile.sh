#!/bin/bash

# Ignore perft.cpp and look for all other files for this compilation
SOURCES=$(find .. -name "*.cpp" ! -name "perft.cpp")

clang++ -std=c++20 -O3 -g -ffast-math \
-mcpu=apple-m4 -flto=thin \
-Xpreprocessor -fopenmp \
-I/opt/homebrew/opt/libomp/include \
-L/opt/homebrew/opt/libomp/lib -lomp \
-Wno-c99-designator \
$SOURCES -o RENAME-ME.bin

# Clear the ignore rule
unset GLOBIGNORE