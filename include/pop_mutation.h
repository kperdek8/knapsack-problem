#ifndef POP_MUTATION_H
#define POP_MUTATION_H
#include "chromosome.h"

struct Item;
enum class MutationMethod { BIT_FLIP, MULTI_BIT_FLIP };

inline std::string to_string(const MutationMethod method) {
  switch(method) {
    case MutationMethod::BIT_FLIP: return "BIT_FLIP";
    case MutationMethod::MULTI_BIT_FLIP: return "MULTI_BIT_FLIP";
    default: return "UNKNOWN";
  }
}

void mutate(Chromosome& chrom, MutationMethod method, float per_bit_chance);
void inverse(Chromosome& chrom);
void repair(Chromosome& chrom, std::span<Item> items, int max_weight);

#endif
