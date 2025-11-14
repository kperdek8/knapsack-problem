#ifndef POP_INIT_H
#define POP_INIT_H

#include <span>
#include "item.h"
#include "chromosome.h"

enum class InitMethod { RANDOM, GREEDY };

void initialize_population(std::span<Chromosome> population,
                           std::span<Item> items,
                           unsigned int chrom_length,
                           int max_weight,
                           InitMethod method = InitMethod::RANDOM);

#endif
