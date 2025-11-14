#include "pop_selection.h"

#include <cstdint>
#include <stdexcept>
#include <vector>
#include "rng.h"
#include "chromosome.h"

Chromosome roulette_select(std::span<Chromosome> population, std::span<const uint64_t> fitness_values,
                    uint64_t total_fitness);

Chromosome tournament_select(std::span<Chromosome> population, std::span<const uint64_t> fitness_values, int tournament_size);

Chromosome select(const std::span<Chromosome> population, const std::span<const uint64_t> fitness_values,
           const uint64_t total_fitness, const int tournament_size, const SelectionMethod method) {
    switch (method) {
        case SelectionMethod::ROULETTE:
            return roulette_select(population, fitness_values, total_fitness);
        case SelectionMethod::TOURNAMENT:
            return tournament_select(population, fitness_values, tournament_size);
        case SelectionMethod::RANK:
            throw std::logic_error("Metoda selekcji RANK nie jest jeszcze zaimplementowana.");
    }
    throw std::logic_error("Do selekcji zostala przekazana nieznana metoda");
}

Chromosome roulette_select(const std::span<Chromosome> population, const std::span<const uint64_t> fitness_values,
                    uint64_t total_fitness) {
    // Wybierz losowego osobnika jeżeli żaden nie jest przystosowany
    if (total_fitness == 0) {
        return population[random_int(0, population.size() - 1)];
    }

    // Losowanie punktu r z zakresu [0, total_fitness)
    const uint64_t r = random_uint64(0, total_fitness - 1);

    // Znajdz osobnika która zawiera punkt r
    uint64_t cumulative = 0;
    for (size_t i = 0; i < population.size(); ++i) {
        cumulative += fitness_values[i];
        if (r < cumulative) {
            return population[i];
        }
    }

    throw std::logic_error("Funkcja metody ruletkowej nie zwróciła poprawnie osobnika");
}

Chromosome tournament_select(const std::span<Chromosome> population, const std::span<const uint64_t> fitness_values, const int tournament_size) {
    size_t best_idx = random_int(0, population.size() - 1);
    uint64_t best_fitness = fitness_values[best_idx];

    // Losowanie osobnikow
    for(size_t i = 0; i < tournament_size; ++i) {
        size_t idx = random_int(0, population.size() - 1);
        if (fitness_values[idx] > best_fitness) {
            best_fitness = fitness_values[idx];
            best_idx = idx;
        }
    }
    return population[best_idx];
}
