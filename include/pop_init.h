#ifndef POP_INIT_H
#define POP_INIT_H

#include <span>
#include "item.h"
#include "chromosome.h"

enum class InitMethod { RANDOM, GREEDY, SINGLE_ITEM };

inline std::string to_string(const InitMethod method) {
  switch(method) {
    case InitMethod::RANDOM: return "RANDOM";
    case InitMethod::GREEDY: return "GREEDY";
    case InitMethod::SINGLE_ITEM: return "SINGLE_ITEM";
    default: return "UNKNOWN";
  }
}

void initialize_population(std::span<Chromosome> population,
                           std::span<Item> items,
                           unsigned int chrom_length,
                           int max_weight,
                           InitMethod method = InitMethod::SINGLE_ITEM);

#endif
