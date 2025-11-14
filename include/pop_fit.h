#ifndef POP_FIT_H
#define POP_FIT_H

#include "item.h"
#include "chromosome.h"
#include <span>

enum class FitMethod {
  ZERO_PENALTY,
  RATIO_PENALTY,
};

uint64_t fitness(std::span<Item> items, const Chromosome& chrom, int max_weight, FitMethod method);

#endif
