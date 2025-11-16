#!/bin/bash

SRC_DIR=$(pwd)
EXE="knapsack"

echo ">>> Building project using build.sh..."
# Budowa aplikacji
"$SRC_DIR/build.sh" || { echo "Build failed"; exit 1; }

FILES=(
    "knapPI_1_50_1000_42.in"
    "knapPI_1_100_1000_20.in"
    "knapPI_1_500_1000_10.in"
)

CROSS_METHODS=("one" "two")
POP=100
CROSS=0.8
MUT=0.1
GEN=250
NO_IMPROVE=20

for file in "${FILES[@]}"; do
    for method in "${CROSS_METHODS[@]}"; do
        for run in {1..100}; do
            echo "Plik: $file | CROSS_METHOD: $method | Run: $run"
            ./$EXE --input "$file" --pop $POP --cross $CROSS --mut $MUT --gen $GEN --no_improve $NO_IMPROVE --cross_method $method
        done
    done
done
