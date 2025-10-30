#include "io_utils.hpp"
#include "item.hpp"
#include "pop_crossover.hpp"
#include "pop_fit.hpp"
#include "pop_init.hpp"
#include "pop_mutation.hpp"
#include "pop_selection.hpp"
#include "rng.hpp"
#include <filesystem>
#include <iostream>
#include <span>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

// W razie problemu z wydajnością zamiast zwracać fitness_values można przyjąć
// referencję jako parametr by uniknać kopiowania listy.
std::tuple<std::vector<int>, int, size_t>
population_fitness(const std::span<int> population, const std::span<Item> items,
                   const int max_weight) {
  int total_fitness = 0;
  int best_fitness = 0;
  size_t best_index = 0;
  std::vector<int> fitness_values(population.size());

  for (size_t i = 0; i < population.size(); ++i) {
    fitness_values[i] =
        fitness(items, population[i], max_weight, FitMethod::RATIO_PENALTY);
    total_fitness += fitness_values[i];
    if (fitness_values[i] > best_fitness) {
      best_fitness = fitness_values[i];
      best_index = i;
    }
  }

  return {fitness_values, total_fitness, best_index};
}

int algorithm(const int max_generations, const int pop_size,
              const int max_no_improvement, const float cross_chance,
              const float mutation_chance, const std::span<Item> items,
              const int max_weight, int output_mask = 0) {
  int best_individual_fitness = 0;
  int generations_without_improvement = 0;
  const unsigned int item_count = items.size();
  const unsigned int chrom_length = item_count; // Alias

  std::vector<int> population{};
  population.resize(pop_size);
  initialize_population(population, item_count, InitMethod::RANDOM);

  // Wektor dla nowej populacji
  std::vector<int> new_population;
  new_population.resize(pop_size);

  // Pierwszy warunek stopu: Limit liczby generacji
  for (int generations = 0; generations < max_generations; ++generations) {
    if (output_mask || io_utils::PRINT_GENERATION) {
      std::cout << "Generacja " << generations << std::endl;
    }
    auto [fitness_values, total_fitness, best_index] = population_fitness(
        population, items, max_weight); // Wyliczenie przystosowania osobników

    int current_best_fitness =
        fitness_values[best_index]; // Przystosowanie najlepszego osobnika z
                                    // populacji

    if (output_mask) {
      io_utils::print_population_stats(output_mask, population, chrom_length,
                                       total_fitness, best_index,
                                       current_best_fitness);
    }

    if (current_best_fitness > best_individual_fitness) {
      best_individual_fitness = current_best_fitness;
      generations_without_improvement = 0; // Reset
    } else {
      ++generations_without_improvement;
    }

    if (generations_without_improvement >= max_no_improvement)
      break; // Drugi warunek stopu: brak poprawy najlepszego rozwiązania

    for (size_t i = 0; i < pop_size; i += 2) {
      int parent1 = select(population, fitness_values, total_fitness, SelectionMethod::ROULETTE);
      int parent2 = select(population, fitness_values, total_fitness, SelectionMethod::ROULETTE);

      // Krzyżowanie jednopunktowe
      auto [children1, children2] =
          (random_float() < cross_chance)
              ? crossover(parent1, parent2, chrom_length,
                          CrossoverMethod::ONE_POINT)
              : std::make_pair(parent1, parent2);

      // Mutacje
      if (random_float() < mutation_chance)
        children1 = mutate(children1, chrom_length, MutationMethod::BIT_FLIP);
      if (random_float() < mutation_chance)
        children2 = mutate(children2, chrom_length, MutationMethod::BIT_FLIP);

      // Dodaj potomków do nowej populacji
      new_population[i] = children1;
      if (i + 1 < pop_size)
        new_population[i + 1] = children2;
    }

    // Zamiana populacji
    population = new_population;
  }

  return best_individual_fitness;
}

int main(int argc, char *argv[]) {

  if (argc < 2) {
    std::cerr << "Uzycie: " << argv[0]
              << " <plik_wejsciowy> [POP_SIZE CROSS_CHANCE MUTATION_CHANCE "
                 "MAX_GENERATIONS MAX_NO_IMPROVEMENT]"
              << std::endl;
    return 1;
  }

  std::string input_file = argv[1];

  // Domyślne wartości
  int POP_SIZE = 100;
  float CROSS_CHANCE = 0.85f;
  float MUTATION_CHANCE = 0.1f;
  int MAX_GENERATIONS = 50;
  int MAX_NO_IMPROVEMENT = 20;
  int debug_mask = io_utils::PRINT_SUMMARY | io_utils::PRINT_SUMMARY |
                   io_utils::PRINT_AVG |
                   // io_utils::PRINT_BEST_CHROM |
                   io_utils::PRINT_BEST_FITNESS;

  // Nadpisz parametry jeśli zostały podane
  if (argc > 2)
    POP_SIZE = std::atoi(argv[2]);
  if (argc > 3)
    CROSS_CHANCE = std::atof(argv[3]);
  if (argc > 4)
    MUTATION_CHANCE = std::atof(argv[4]);
  if (argc > 5)
    MAX_GENERATIONS = std::atoi(argv[5]);
  if (argc > 6)
    MAX_NO_IMPROVEMENT = std::atoi(argv[6]);

  // Wczytanie danych
  int max_weight, optimal_value;
  auto items = io_utils::load_items(input_file, max_weight, optimal_value);

  // Algorytm
  int best_fitness =
      algorithm(MAX_GENERATIONS, POP_SIZE, MAX_NO_IMPROVEMENT, CROSS_CHANCE,
                MUTATION_CHANCE, items, max_weight, debug_mask);

  // Wypisanie do konsoli
  if (debug_mask | io_utils::PRINT_SUMMARY) {
    std::cout << "=========================================================="
              << std::endl;
    std::cout << "POP_SIZE CROSS_CHANCE MUTATION_CHANCE MAX_GENERATIONS "
                 "MAX_NO_IMPROVEMENT BEST_FIT BEST_FIT_PER"
              << std::endl;
    std::cout << POP_SIZE << " " << CROSS_CHANCE << " " << MUTATION_CHANCE
              << " " << MAX_GENERATIONS << " " << MAX_NO_IMPROVEMENT << " "
              << best_fitness << " "
              << static_cast<float>(best_fitness) / optimal_value << std::endl;
    std::cout << "Najlepsze przystosowanie (wszystkie populacje): "
              << best_fitness << std::endl;
    std::cout << "Optymalne rozwiazanie: " << optimal_value << std::endl;
    std::cout << "Zblizenie do optymalnego rozwiazania: "
              << static_cast<float>(best_fitness) / optimal_value << std::endl;
  }

  // Zapis do pliku .out w formacie csv
  std::filesystem::path out_file = input_file;
  out_file.replace_extension(".csv");

  io_utils::log_results_to_csv(
      out_file.string(), POP_SIZE, CROSS_CHANCE, MUTATION_CHANCE,
      MAX_GENERATIONS, MAX_NO_IMPROVEMENT, best_fitness,
      static_cast<float>(best_fitness) / optimal_value);

  return 0;
}

/* Źródło danych https://hjemmesider.diku.dk/~pisinger/codes.html
 * Zbiory wybrano ze względu na liczbe przedmiotów (N=50, 100, 500) oraz
 * maksymalną wagę plecaka (C=1000,10000,25000) Uwzględniono tylko jedną
 * maksymalną wagę przedmiotów równą 1000 Format wejściowy dostosowano do
 * łatwiejszego wczytywania
 *
 * Użyte zbiory:
 * knapPI_1_50_1000_1 knapPI_1_50_1000_42 knapPI_1_50_1000_85
 * knapPI_1_100_1000_1 knapPI_1_100_1000_20 knapPI_1_100_1000_52
 * knapPI_1_500_1000_1 knapPI_1_500_1000_4 knapPI_1_500_1000_10
 */
