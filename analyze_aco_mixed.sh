#!/bin/bash

SRC_DIR=$(pwd)
EXE="knapsack_ant"

echo ">>> Building project using build.sh..."
# Budowa aplikacji
"$SRC_DIR/build.sh" || { echo "Build failed"; exit 1; }

FILES=(
    "knapPI_1_50_1000_42.in"
    "knapPI_1_100_1000_20.in"
    "knapPI_1_500_1000_10.in"
)

POP=100
ALPHA=1.5
BETA=3.0
EVAP=0.2
PHEROMONE_INIT=1.0
NOIMP=50
MAX_GENERATIONS=(100)

for file in "${FILES[@]}"; do
    for gen in "${MAX_GENERATIONS[@]}"; do
        for run in {1..100}; do
            echo "Plik: $file | Run: $run"
            ./$EXE --input $file --pop $POP --pheromone_influence $ALPHA --heuristic_influence $BETA --evaporation_rate $EVAP --pheromone_init $PHEROMONE_INIT --gen $gen --no_improve $NOIMP
        done
    done
done