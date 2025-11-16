#!/bin/bash

SRC_DIR=$(pwd)
EXE="knapsack"

echo ">>> Building project using build.sh..."
# Uruchamiamy build.sh
"$SRC_DIR/build.sh" || { echo "Build failed"; exit 1; }

FILES=(
    "knapPI_1_50_1000_1.in"
    "knapPI_1_50_1000_42.in"
    "knapPI_1_50_1000_85.in"
    "knapPI_1_100_1000_1.in"
    "knapPI_1_100_1000_20.in"
    "knapPI_1_100_1000_52.in"
    "knapPI_1_500_1000_1.in"
    "knapPI_1_500_1000_4.in"
    "knapPI_1_500_1000_10.in"
)

# Parametry
POP_SIZES=(20 50 100 200)
CROSS_CHANCES=(0.7 0.8 0.9)
MUTATION_CHANCES=(0.05 0.1 0.15)
MAX_GENERATIONS=(50 100 250)
MAX_NO_IMPROVEMENT=(20)

# Wywołanie każdej kombinacji
for file in "${FILES[@]}"; do
    for pop in "${POP_SIZES[@]}"; do
        for cross in "${CROSS_CHANCES[@]}"; do
            for mut in "${MUTATION_CHANCES[@]}"; do
                for gen in "${MAX_GENERATIONS[@]}"; do
                    for noimp in "${MAX_NO_IMPROVEMENT[@]}"; do
                        # Trzykrotne uruchomienie
                        for run in {1..3}; do
                            echo "Plik: $file | POP_SIZE: $pop | CROSS_CHANCE: $cross | MUTATION_CHANCE: $mut | MAX_GEN: $gen | MAX_NO_IMP: $noimp | Run: $run"
                            ./$EXE "$file" --pop $pop --cross $cross --mut $mut --gen $gen --no_improve $noimp
                        done
                    done
                done
            done
        done
    done
done
