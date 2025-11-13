#include "pop_selection.hpp"

#include <stdexcept>

#include "rng.hpp"

int roulette_select(std::span<int> population, std::span<const int> fitness_values,
                    int total_fitness);

int select(const std::span<int> population, const std::span<const int> fitness_values,
           int total_fitness, SelectionMethod method) {
    switch (method) {
        case SelectionMethod::ROULETTE:
            return roulette_select(population, fitness_values, total_fitness);
        case SelectionMethod::TOURNAMENT:
            throw std::logic_error("Metoda selekcji TOURNAMENT nie jest jeszcze zaimplementowana.");
        case SelectionMethod::RANK:
            throw std::logic_error("Metoda selekcji RANK nie jest jeszcze zaimplementowana.");
    }
    throw std::logic_error("Do selekcji zostala przekazana nieznana metoda");
}

int roulette_select(const std::span<int> population, const std::span<const int> fitness_values,
                    int total_fitness) {
    // Wybierz losowego osobnika jeżeli żaden nie jest przystosowany
    if (total_fitness == 0) {
        return population[random_int(0, population.size() - 1)];
    }

    // Losowanie punktu r z zakresu [0, total_fitness)
    int r = random_int(0, total_fitness - 1);

    // Znajdz osobnika która zawiera punkt r
    int cumulative = 0;
    for (size_t i = 0; i < population.size(); ++i) {
        cumulative += fitness_values[i];
        if (r < cumulative) {
            return population[i];
        }
    }

    throw std::logic_error("Funkcja metody ruletkowej nie zwróciła poprawnie osobnika");
}
