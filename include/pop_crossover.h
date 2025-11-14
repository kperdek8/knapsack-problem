#ifndef POP_CROSSOVER_H
#define POP_CROSSOVER_H
#include <utility>
#include "chromosome.h"

enum class CrossoverMethod { ONE_POINT, TWO_POINT, UNIFORM };

std::pair<Chromosome, Chromosome> crossover(const Chromosome& parent1, const Chromosome& parent2, CrossoverMethod method);

#endif
