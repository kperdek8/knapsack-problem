#ifndef POP_CROSSOVER_H
#define POP_CROSSOVER_H
#include <utility>
#include "chromosome.h"

enum class CrossoverMethod { ONE_POINT, TWO_POINT, UNIFORM };

inline std::string to_string(const CrossoverMethod method) {
  switch(method) {
    case CrossoverMethod::ONE_POINT: return "ONE_POINT";
    case CrossoverMethod::TWO_POINT: return "TWO_POINT";
    default: return "UNKNOWN";
  }
}

std::pair<Chromosome, Chromosome> crossover(const Chromosome& parent1, const Chromosome& parent2, CrossoverMethod method);

#endif
