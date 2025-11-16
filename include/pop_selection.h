#ifndef POP_SELECTION_H
#define POP_SELECTION_H
#include <span>
#include "chromosome.h"

enum class SelectionMethod {
  ROULETTE,
  TOURNAMENT,
  RANK,
};

inline std::string to_string(const SelectionMethod method) {
  switch(method) {
    case SelectionMethod::ROULETTE: return "ROULETTE";
    case SelectionMethod::TOURNAMENT: return "TOURNAMENT";
    default: return "UNKNOWN";
  }
}

Chromosome select(std::span<Chromosome> population,
                    std::span<const uint64_t> fitness_values,
                    uint64_t total_fitness, int tournament_size, SelectionMethod method);

#endif
