#ifndef POP_CROSSOVER_H
#define POP_CROSSOVER_H
#include <utility>

enum class CrossoverMethod { ONE_POINT, TWO_POINT, UNIFORM };

std::pair<int, int> crossover(int parent1, int parent2,
                              unsigned int chrom_length,
                              CrossoverMethod method);

#endif
