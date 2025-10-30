#ifndef POP_MUTATION_H
#define POP_MUTATION_H

enum class MutationMethod { BIT_FLIP, MULTI_BIT_FLIP };

int mutate(const int chrom, const unsigned int chrom_length, MutationMethod method);

#endif
