#ifndef POP_INIT_H
#define POP_INIT_H

#include <span>

enum class InitMethod { RANDOM, GREEDY };

void initialize_population(std::span<int> population,
                           unsigned int chrom_length,
                           InitMethod method = InitMethod::RANDOM);

#endif
