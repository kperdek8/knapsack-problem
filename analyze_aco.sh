#!/bin/bash

SRC_DIR=$(pwd)
EXE="knapsack_ant"

echo ">>> Building project using build.sh..."
# Budowa aplikacji
"$SRC_DIR/build.sh" || { echo "Build failed"; exit 1; }

FILES=(
#    "knapPI_1_50_1000_1.in"
    "knapPI_1_50_1000_42.in"
#    "knapPI_1_50_1000_85.in"
#    "knapPI_1_100_1000_1.in"
    "knapPI_1_100_1000_20.in"
#    "knapPI_1_100_1000_52.in"
#    "knapPI_1_500_1000_1.in"
#    "knapPI_1_500_1000_4.in"
    "knapPI_1_500_1000_10.in"
)

echo "--- SEKCJA 1: TESTOWANIE WPLYWU ALFA/BETA ---"

POP=50
EVAP=0.1
PHEROMONE_INIT=1.0
GEN=100
NOIMP=50

ALPHA=(0 0.5 1.0 1.5 2.0 3.0)
BETA=(0 0.5 1.0 1.5 2.0 3.0)

for file in "${FILES[@]}"; do
    for alpha in "${ALPHA[@]}"; do
        for beta in "${BETA[@]}"; do
            for run in {1..10}; do
                echo "Plik: $file | Zmiana ALFA: $alpha | Zmiana BETA: $beta | Run: $run"
                ./$EXE --input $file --pop $POP --pheromone_influence $alpha --heuristic_influence $beta --evaporation_rate $EVAP --pheromone_init $PHEROMONE_INIT --gen $GEN --no_improve $NOIMP
            done
        done
    done
done

echo "--- SEKCJA 2: TESTOWANIE INICJALIZACJI FEROMONU ---"

# Stałe parametry dla tej sekcji
POP=50
ALPHA=1.0
BETA=1.0
EVAP=0.1
GEN=100
NOIMP=50
INITIAL_PHEROMONE=(0.01 0.1 1.0 10.0 100.0)

for file in "${FILES[@]}"; do
    for init in "${INITIAL_PHEROMONE[@]}"; do
        for run in {1..10}; do
            echo "Plik: $file | Zmiana PHEROMONE_INIT: $init | Run: $run"
            ./$EXE --input $file --pop $POP --pheromone_influence $ALPHA --heuristic_influence $BETA --evaporation_rate $EVAP --pheromone_init $init --gen $GEN --no_improve $NOIMP
        done
    done
done

echo "--- SEKCJA 3: TESTOWANIE WPLYWU PAROWANIA ---"

POP=50
ALPHA=1.0
BETA=1.0
PHEROMONE_INIT=1.0
GEN=100
NOIMP=50
EVAPORATION_RATE=(0.01 0.05 0.1 0.2 0.5)

for file in "${FILES[@]}"; do
    for evap in "${EVAPORATION_RATE[@]}"; do
        for run in {1..10}; do
            echo "Plik: $file | Zmiana EVAPORATION_RATE: $evap | Run: $run"
            ./$EXE --input $file --pop $POP --pheromone_influence $ALPHA --heuristic_influence $BETA --evaporation_rate $evap --pheromone_init $PHEROMONE_INIT --gen $GEN --no_improve $NOIMP
        done
    done
done

echo "--- SEKCJA 4: TESTOWANIE WPLYWU ROZMIARU POPULACJI ---"

ALPHA=1.0
BETA=1.0
EVAP=0.1
PHEROMONE_INIT=1.0
GEN=100
NOIMP=50
POP_SIZES=(10 20 50 100 200)

for file in "${FILES[@]}"; do
    for pop in "${POP_SIZES[@]}"; do
        for run in {1..5}; do
            echo "Plik: $file | Zmiana POP_SIZE: $pop | Run: $run"
            ./$EXE --input $file --pop $pop --pheromone_influence $ALPHA --heuristic_influence $BETA --evaporation_rate $EVAP --pheromone_init $PHEROMONE_INIT --gen $GEN --no_improve $NOIMP
        done
    done
done

echo "--- SEKCJA 5: TESTOWANIE WPLYWU LICZBY GENERACJI (MAX_GENERATIONS) ---"

POP=50
ALPHA=1.0
BETA=1.0
EVAP=0.1
PHEROMONE_INIT=1.0
NOIMP=50
MAX_GENERATIONS=(50 100 150 200 250)

for file in "${FILES[@]}"; do
    for gen in "${MAX_GENERATIONS[@]}"; do
        for run in {1..5}; do
            echo "Plik: $file | Zmiana MAX_GENERATIONS: $gen | Run: $run"
            ./$EXE --input $file --pop $POP --pheromone_influence $ALPHA --heuristic_influence $BETA --evaporation_rate $EVAP --pheromone_init $PHEROMONE_INIT --gen $gen --no_improve $NOIMP
        done
    done
done