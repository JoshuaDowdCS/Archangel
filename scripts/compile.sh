#!/bin/bash

# Ignore perft.cpp for this compilation
export GLOBIGNORE="perft.cpp"

clang++ -std=c++20 -O3 -ffast-math \
-mcpu=apple-m4 -flto=thin \
-Xpreprocessor -fopenmp \
-I/opt/homebrew/opt/libomp/include \
-L/opt/homebrew/opt/libomp/lib -lomp \
*.cpp -o RENAME-ME.bin

# Clear the ignore rule
unset GLOBIGNORE