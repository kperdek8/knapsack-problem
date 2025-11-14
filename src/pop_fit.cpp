#include "pop_fit.h"

#include <iostream>

// Pierwszy przedmiot = najmłodszy bit (z prawej)
uint64_t fitness(const std::span<Item> items, const Chromosome& chrom, const int max_weight, const FitMethod method) {
    uint64_t total_weight = 0;
    uint64_t total_value = 0;

    for (size_t i = 0; i < items.size(); ++i) {
        if (chrom[i]) {
            total_weight += items[i].weight;
            total_value += items[i].value;
        }
    }

    if (total_weight > max_weight)
        switch (method) {
            case FitMethod::ZERO_PENALTY:
                return 0;
            case FitMethod::RATIO_PENALTY:
                return total_value * max_weight / total_weight;
        };

    return total_value;
}
