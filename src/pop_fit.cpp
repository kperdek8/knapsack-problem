#include "pop_fit.hpp"

// Pierwszy przedmiot = najmłodszy bit (z prawej)
int fitness(const std::span<Item> items, const int chrom, const int max_weight, FitMethod method) {
    int total_weight = 0;
    int total_value = 0;

    for (size_t i = 0; i < items.size(); ++i) {
        if (chrom & (1ULL << i)) {
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
