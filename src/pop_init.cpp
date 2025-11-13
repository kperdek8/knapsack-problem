#include "pop_init.hpp"

#include <cmath>
#include <stdexcept>

#include "rng.hpp"

void initialize_population(std::span<int> population, const unsigned int chrom_length,
                           InitMethod method) {
    switch (method) {
        case InitMethod::RANDOM:
            for (int& individual : population) {
                individual = random_int(0, pow(2, chrom_length) - 1);
            }
            break;
        case InitMethod::GREEDY:
            throw std::logic_error("Metoda zachłanna nie jest jeszcze zaimplementowana.");
            break;
    }
}
