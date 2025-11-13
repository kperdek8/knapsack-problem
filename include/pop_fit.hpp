#ifndef POP_FIT_H
#define POP_FIT_H

#include "item.hpp"
#include <span>

enum class FitMethod {
  ZERO_PENALTY,
  RATIO_PENALTY,
};

int fitness(std::span<Item> items, int chrom, int max_weight,
            FitMethod method);

#endif
