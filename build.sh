#!/bin/bash

SRC_DIR=$(pwd)
BUILD_DIR="$SRC_DIR/build"
EXE="knapsack"

echo ">>> Configuring and building with CMake..."
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR" || exit 1

cmake -DCMAKE_BUILD_TYPE=Release "$SRC_DIR" || { echo "CMake configuration failed"; exit 1; }

cmake --build . || { echo "Build failed"; exit 1; }

cp "$BUILD_DIR/$EXE" "$SRC_DIR/" || { echo "Failed to copy executable"; exit 1; }
cd "$SRC_DIR" || exit 1