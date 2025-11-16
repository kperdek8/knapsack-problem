#ifndef IO_UTILS_H
#define IO_UTILS_H

#include <string>
#include <span>
#include <vector>
#include "item.h"
#include "parser.h"
#include "chromosome.h"

namespace io_utils {

    enum PopulationOptions {
        PRINT_AVG = 1 << 0,
        PRINT_BEST_CHROM = 1 << 1,
        PRINT_BEST_FITNESS = 1 << 2,
        PRINT_GENERATION = 1 << 3,
        PRINT_SUMMARY = 1 << 4,
        PRINT_FINAL_BEST_CHROM = 1 << 5,
        PRINT_FINAL_BEST_FITNESS = 1 << 6,
    };
    // INPUT
    std::vector<Item> load_items(const std::string& filename, int& max_weight, int& optimal_value);

    // OUTPUT
    void log_results_to_csv(const std::string& filename, const ProgramArgs& args, uint64_t best_fitness,
                            float best_fitness_ratio, float avg_population_fitness_ratio);
    void print_population(std::span<Chromosome> population);
    void print_population_stats(int options,
        const std::span<Chromosome>& pop, uint64_t total_fitness, size_t best_index, uint64_t current_best_fitness
    );
}

#endif
