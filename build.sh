#!/bin/bash

SRC_DIR=$(pwd)
BUILD_DIR="$SRC_DIR/build"

EXES=("knapsack" "knapsack_ant")

echo ">>> Configuring and building with CMake..."
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR" || exit 1

cmake -DCMAKE_BUILD_TYPE=Release "$SRC_DIR" || { echo "CMake configuration failed"; exit 1; }

cmake --build . || { echo "Build failed"; exit 1; }

echo ">>> Copying executables..."

cd "$SRC_DIR" || exit 1

for exe in "${EXES[@]}"; do
    if [[ -f "$BUILD_DIR/$exe" ]]; then
        cp "$BUILD_DIR/$exe" "$SRC_DIR/" && echo "Copied $exe"
    else
        echo "Warning: Executable $exe not found in build folder"
    fi
done

echo ">>> Done."