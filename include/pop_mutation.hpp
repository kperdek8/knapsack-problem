#ifndef POP_MUTATION_H
#define POP_MUTATION_H
#include <string>

enum class MutationMethod { BIT_FLIP, MULTI_BIT_FLIP };

int mutate(int chrom, unsigned int chrom_length, MutationMethod method, float per_bit_chance);

#endif
