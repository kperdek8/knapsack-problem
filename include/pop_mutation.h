#ifndef POP_MUTATION_H
#define POP_MUTATION_H
#include "chromosome.h"

struct Item;
enum class MutationMethod { BIT_FLIP, MULTI_BIT_FLIP };

void mutate(Chromosome& chrom, MutationMethod method, float per_bit_chance);
void inverse(Chromosome& chrom);
void repair(Chromosome& chrom, std::span<Item> items, int max_weight);

#endif
