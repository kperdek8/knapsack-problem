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

SELECTION_METHODS=("roulette" "tournament")
POP=100
CROSS=0.8
MUT=0.1
GEN=250
NO_IMPROVE=20
TOURNAMENT_SIZES=(3 5 7)

for file in "${FILES[@]}"; do
    for method in "${SELECTION_METHODS[@]}"; do
        if [ "$method" == "tournament" ]; then
            for t_size in "${TOURNAMENT_SIZES[@]}"; do
                for run in {1..100}; do
                    echo "Plik: $file | SELECTION: $method | TOURNAMENT_SIZE: $t_size | Run: $run"
                    ./$EXE --input "$file" --pop $POP --cross $CROSS --mut $MUT --gen $GEN --no_improve $NO_IMPROVE --selection $method --tournament_size $t_size
                done
            done
        else
            for run in {1..100}; do
                echo "Plik: $file | SELECTION: $method | Run: $run"
                ./$EXE --input "$file" --pop $POP --cross $CROSS --mut $MUT --gen $GEN --no_improve $NO_IMPROVE --selection $method
            done
        fi
    done
done
