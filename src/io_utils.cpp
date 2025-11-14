#include "io_utils.h"

#include <filesystem>
#include <fstream>
#include <iostream>

#include "helper.h"

// INPUT

std::vector<Item> io_utils::load_items(const std::string& filename, int& max_weight,
                                       int& optimal_value) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Nie mozna otworzyc pliku: " + filename);
    }

    int n;
    file >> n;              // liczba przedmiotów
    file >> max_weight;     // maksymalna pojemność
    file >> optimal_value;  // optymalna wartość

    std::vector<Item> items;
    items.reserve(n);

    std::string line;
    std::getline(file, line);  // czyść bufor po ostatnim znaku nowej linii (linia pusta)

    for (int i = 0; i < n; ++i) {
        if (!std::getline(file, line))
            break;

        std::istringstream iss(line);

        int in_optimal_flag;
        int value, weight;

        iss >> value >> weight >> in_optimal_flag;
        // item.in_optimal = (in_optimal_flag != 0); // możliwość dodania flagi z optymalnym
        // rozwiązaniem w razie potrzeby
        items.emplace_back(value, weight);
    }

    file.close();
    return items;
}

// OUTPUT

void io_utils::print_population(const std::span<Chromosome> population) {
    for (const auto& individual : population) {
        std::cout << individual.to_string() << " ";
    }
    std::cout << std::endl;
}

void io_utils::print_population_stats(int options, const std::span<Chromosome>& pop,
                                      uint64_t total_fitness, size_t best_index,
                                      uint64_t current_best_fitness) {
    if (options & io_utils::PRINT_AVG)
        std::cout << "Srednie przystosowanie nowej populacji: " << total_fitness / pop.size()
                  << "\n";
    if (options & io_utils::PRINT_BEST_CHROM)
        std::cout << "Najlepszy osobnik: " << pop[best_index].to_string()
                  << "\n";
    if (options & io_utils::PRINT_BEST_FITNESS)
        std::cout << "Najlepsze przystosowanie: " << current_best_fitness << "\n";
}

void io_utils::log_results_to_csv(const std::string& filename, const ProgramArgs& args,
                                  uint64_t best_fitness, float best_fitness_ratio) {
    namespace fs = std::filesystem;

    bool file_exists = fs::exists(filename);

    std::ofstream file(filename, std::ios::app);  // append mode
    if (!file.is_open()) {
        std::cerr << "Nie mozna otworzyc pliku: " << filename << std::endl;
        return;
    }

    if (!file_exists) {
        file << "POP_SIZE,CROSS_CHANCE,MUTATION_CHANCE,PER_GENE_MUTATION_CHANCE,MAX_GENERATIONS,"
                "MAX_NO_IMPROVEMENT,MUTATION_METHOD,BEST_FIT,BEST_FIT_PER\n";
    }

    const auto mutation_method =
        args.mutation_method == MutationMethod::BIT_FLIP ? "BIT_FLIP" : "MULTI_BIT_FLIP";

    file << args.pop_size << "," << args.cross_chance << "," << args.mutation_chance << ","
         << args.mutate_per_gene << "," << args.max_generations << "," << args.max_no_improvement
         << "," << mutation_method << "," << best_fitness << "," << best_fitness_ratio << "\n";

    file.close();
}
