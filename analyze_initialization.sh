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

INIT_METHODS=("random" "single_item" "greedy")
POP=100
CROSS=0.8
MUT=0.1
GEN=250
NO_IMPROVE=20
REPAIR=0.05

for file in "${FILES[@]}"; do
    for init in "${INIT_METHODS[@]}"; do
        for run in {1..100}; do
            echo "Plik: $file | INIT_METHOD: $init | Run: $run"
            ./$EXE --input "$file" --pop $POP --cross $CROSS --mut $MUT --gen $GEN --no_improve $NO_IMPROVE --init $init --repair $REPAIR
        done
    done
done