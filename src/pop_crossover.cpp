#include "pop_crossover.hpp"
#include "rng.hpp"
#include <stdexcept>

std::pair<int, int> do_one_point(const int parent1, const int parent2,
                                 const unsigned int chrom_length);
std::pair<int, int> do_two_point(const int parent1, const int parent2,
                                 const unsigned int chrom_length);
std::pair<int, int> do_uniform(const int parent1, const int parent2,
                               const unsigned int chrom_length);

std::pair<int, int> crossover(const int parent1, const int parent2,
                              const unsigned int chrom_length,
                              CrossoverMethod method) {
  switch (method) {
  case CrossoverMethod::ONE_POINT:
    return do_one_point(parent1, parent2, chrom_length);
  case CrossoverMethod::TWO_POINT:
    throw std::logic_error(
        "Metoda krzyzowania TWO_POINT nie jest zaimplementowana");
  case CrossoverMethod::UNIFORM:
    throw std::logic_error(
        "Metoda krzyzowania UNIFORM nie jest zaimplementowana");
  }
}

// Punkt podziału liczony od prawej z indeksowanem od zera
std::pair<int, int> do_one_point(const int parent1, const int parent2,
                                 const unsigned int chrom_length) {
  const int crossover_bit =
      random_int(1, chrom_length - 1); // n = 0 lub n=chrom_length sprawiły że
                                       // dzieci byłyby jednakowe do rodziców
  const int lower_mask = (1 << crossover_bit) - 1;
  const int upper_mask = ((1 << chrom_length) - 1) ^ lower_mask;

  int child1 = (parent1 & lower_mask) | (parent2 & upper_mask);
  int child2 = (parent2 & lower_mask) | (parent1 & upper_mask);

  return {child1, child2};
}
