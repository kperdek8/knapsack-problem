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

MUTATION_METHODS=("single" "multi")
POP=100
CROSS=0.8
MUT=0.1
GEN=250
NO_IMPROVE=20
MUT_PER_GENE=(0.005 0.01 0.015)

for file in "${FILES[@]}"; do
    for method in "${MUTATION_METHODS[@]}"; do
        for mut_per_gene in "${MUT_PER_GENE[@]}"; do
            for run in {1..100}; do
                echo "Plik: $file | MUTATION_METHOD: $method | MUT_PER_GENE_CHANCE: $mut_per_gene | Run: $run"
                ./$EXE --input "$file" --pop $POP --cross $CROSS --mut $MUT --gen $GEN --no_improve $NO_IMPROVE --mutation $method --mut_per_gene $mut_per_gene
            done
        done
    done
done
