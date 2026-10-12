#!/bin/bash

WARNINGS=(-Wno-c99-designator)

for arg in "$@"; do
    case "$arg" in
        Warn) WARNINGS=(-Weverything) ;;
        *)
            echo "Usage: $0 [Warn]" >&2
            exit 1
            ;;
    esac
done

# Ignore perft.cpp and collect all other C++ files.
SOURCES=()
while IFS= read -r -d '' file; do
    SOURCES+=("$file")
done < <(find .. -name "*.cpp" ! -name "perft.cpp" -print0)

clang++ -std=c++20 -O3 -ffast-math \
    -mcpu=apple-m4 -flto=thin \
    -Xpreprocessor -fopenmp \
    -I/opt/homebrew/opt/libomp/include \
    -L/opt/homebrew/opt/libomp/lib -lomp \
    "${WARNINGS[@]}" \
    "${SOURCES[@]}" -o RENAME-ME.bin