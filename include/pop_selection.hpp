#ifndef POP_SELECTION_H
#define POP_SELECTION_H
#include <span>

enum class SelectionMethod {
  ROULETTE,
  TOURNAMENT,
  RANK,
};

int select(std::span<int> population,
                    std::span<const int> fitness_values,
                    int total_fitness, SelectionMethod method);

#endif
