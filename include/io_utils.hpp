#ifndef IO_UTILS_H
#define IO_UTILS_H

#include <string>
#include <span>
#include <vector>
#include "item.hpp"
#include "parser.hpp"

namespace io_utils {

    enum PopulationOptions {
        PRINT_AVG = 1 << 0,
        PRINT_BEST_CHROM = 1 << 1,
        PRINT_BEST_FITNESS = 1 << 2,
        PRINT_GENERATION = 1 << 3,
        PRINT_SUMMARY = 1 << 4,
    };
    // INPUT
    std::vector<Item> load_items(const std::string& filename, int& max_weight, int& optimal_value);

    // OUTPUT
    void log_results_to_csv(const std::string& filename,
                            const ProgramArgs& args, int best_fitness, float best_fitness_ratio);
    void print_population(std::span<int> population, unsigned int chrom_length);
    void print_population_stats(int options,
        const std::span<int>& pop, int chrom_length, double total_fitness, int best_index, double current_best_fitness
    );
}

#endif
