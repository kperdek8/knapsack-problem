#include "pop_init.h"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <numeric>
#include <vector>

#include "rng.h"

void initialize_population(std::span<Chromosome> population, std::span<Item> items,
                           const unsigned int chrom_length, int max_weight, InitMethod method) {
    switch (method) {
        case InitMethod::RANDOM:
            for (auto& individual : population) {
                individual = Chromosome::random(chrom_length);
            }
            break;

        case InitMethod::GREEDY:
            std::vector<int> indices(chrom_length);
            std::iota(indices.begin(), indices.end(), 0);
            std::ranges::sort(indices.begin(), indices.end(), [&](int a, int b) {
                // Sortowanie wedlug stosunku wartości do wagi
                double va = static_cast<double>(items[a].value) / items[a].weight;
                double vb = static_cast<double>(items[b].value) / items[b].weight;
                return va > vb;
            });

            for (auto& individual : population) {
                individual = Chromosome(chrom_length);
                int total_weight = 0;

                // losowy start w posortowanej liście w celu odroznienia osobnikow
                int start = random_int(0, chrom_length - 1);

                for (int offset = 0; offset < chrom_length; ++offset) {
                    int idx = indices[(start + offset) % chrom_length];
                    if (total_weight + items[idx].weight <= max_weight) {
                        individual.set(idx, true);
                        total_weight += items[idx].weight;
                    }
                }
            }
            break;
    }
}
