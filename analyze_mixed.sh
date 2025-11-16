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

for file in "${FILES[@]}"; do
    for run in {1..100}; do
        echo "Plik: $file | Run: $run"
        ./$EXE --input "$file" --pop 100 --cross 0.8 --mut 0.1 --gen 250 --no_improve 20 --selection tournament --tournament_size 5 --cross_method two --mutation multi --mut_per_gene 0.01 --repair 0.1  --init single_item
    done
done
