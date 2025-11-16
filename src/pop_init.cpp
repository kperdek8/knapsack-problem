#include "pop_init.h"

#include <algorithm>
#include <numeric>
#include <vector>

#include "rng.h"

void initialize_population(std::span<Chromosome> population, const std::span<Item> items,
                           const unsigned int chrom_length, const int max_weight, const InitMethod method) {
    switch (method) {
        case InitMethod::RANDOM:
            for (auto& individual : population) {
                individual = Chromosome::random(chrom_length);
            }
            break;
        case InitMethod::SINGLE_ITEM:
            for (size_t i = 0; i < population.size(); ++i) {
                population[i] = Chromosome(chrom_length);
                // n-ty osobnik dostaje n-ty przedmiot, zapetlone w przypadku pop > items
                const size_t index = i % chrom_length;
                population[i].set(index, true);
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
