#ifndef POP_CROSSOVER_H
#define POP_CROSSOVER_H
#include <utility>

enum class CrossoverMethod { ONE_POINT, TWO_POINT, UNIFORM };

std::pair<int, int> crossover(const int parent1, const int parent2,
                              const unsigned int chrom_length,
                              CrossoverMethod method);

#endif
