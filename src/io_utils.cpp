#include <filesystem>
#include <fstream>
#include <iostream>
#include "io_utils.hpp"
#include "helper.hpp"

// INPUT

std::vector<Item> io_utils::load_items(const std::string& filename, int& max_weight, int& optimal_value) {
  std::ifstream file(filename);
  if (!file.is_open()) {
    throw std::runtime_error("Nie mozna otworzyc pliku: " + filename);
  }

  int n;
  file >> n;               // liczba przedmiotów
  file >> max_weight;      // maksymalna pojemność
  file >> optimal_value;   // optymalna wartość

  std::vector<Item> items;
  items.reserve(n);

  std::string line;
  std::getline(file, line); // czyść bufor po ostatnim znaku nowej linii (linia pusta)

  for (int i = 0; i < n; ++i) {
    if (!std::getline(file, line)) break;

    std::istringstream iss(line);

    int in_optimal_flag;
    int value, weight;

    iss >> value >> weight >> in_optimal_flag;
    // item.in_optimal = (in_optimal_flag != 0); // możliwość dodania flagi z optymalnym rozwiązaniem w razie potrzeby
    items.emplace_back(value, weight);
  }

  file.close();
  return items;
}

// OUTPUT

void io_utils::print_population(std::span<int> population, const unsigned int chrom_length) {
  for(const int& individual : population) {
    std::cout<<to_binary_string(individual, chrom_length)<<" ";
  }
  std::cout<<std::endl;
}


void io_utils::print_population_stats(int options,
    const std::span<int>& pop, int chrom_length, double total_fitness, int best_index, double current_best_fitness
) {
    if (options & io_utils::PRINT_AVG)
        std::cout << "Srednie przystosowanie nowej populacji: "
                  << total_fitness / pop.size() << "\n";
    if (options & io_utils::PRINT_BEST_CHROM)
        std::cout << "Najlepszy osobnik: "
                  << to_binary_string(pop[best_index], chrom_length) << "\n";
    if (options & io_utils::PRINT_BEST_FITNESS)
        std::cout << "Najlepsze przystosowanie: "
                  << current_best_fitness << "\n";
}

void io_utils::log_results_to_csv(const std::string& filename,
                        int pop_size, float cross_chance, float mutation_chance,
                        int max_generations, int max_no_improvement,
                        int best_fitness, float best_fitness_ratio)
{
  namespace fs = std::filesystem;

  bool file_exists = fs::exists(filename);

  std::ofstream file(filename, std::ios::app); // append mode
  if (!file.is_open()) {
    std::cerr << "Nie mozna otworzyc pliku: " << filename << std::endl;
    return;
  }

  if (!file_exists) {
    file << "POP_SIZE,CROSS_CHANCE,MUTATION_CHANCE,MAX_GENERATIONS,MAX_NO_IMPROVEMENT,BEST_FIT,BEST_FIT_PER\n";
  }

  file << pop_size << ","
       << cross_chance << ","
       << mutation_chance << ","
       << max_generations << ","
       << max_no_improvement << ","
       << best_fitness << ","
       << best_fitness_ratio
       << "\n";

  file.close();
}
