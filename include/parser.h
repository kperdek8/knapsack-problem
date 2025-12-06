#ifndef PARSER_H
#define PARSER_H

#include <pop_crossover.h>
#include <pop_fit.h>

#include <string>

#include "pop_init.h"
#include "pop_mutation.h"
#include "pop_selection.h"

enum class AlgorithmMode {
  GA,
  ACO
};

struct ProgramArgs {
    std::string input_file;
    AlgorithmMode mode = AlgorithmMode::GA;
    int pop_size = 100;
    int max_generations = 50;
    int max_no_improvement = 20;
    InitMethod initialization_method = InitMethod::SINGLE_ITEM;
    FitMethod fit_method = FitMethod::ZERO_PENALTY;
    bool debug_output = false;
    // GA
    float cross_chance = 0.85f;
    float inversion_chance = 0.05f;
    float repair_chance = 0.05f;
    float mutation_chance = 0.1f;
    int elites = 0;
    float mutate_per_gene = 0.005f;
    int tournament_size = 5;
    bool inversion_enabled = false;
    bool repair_enabled = false;
    CrossoverMethod crossover_method = CrossoverMethod::ONE_POINT;
    MutationMethod mutation_method = MutationMethod::BIT_FLIP;
    SelectionMethod selection_method = SelectionMethod::ROULETTE;
    // ACO
    float pheromone_influence = 1.0f;
    float heuristic_influence = 1.0f;
    float evaporation_rate = 0.1f;
    float pheromone_init = 0.1f;
    float reinforcement_constant = 1.0f;
};

class Parser {
   public:
    static ProgramArgs parse(int argc, char* argv[], AlgorithmMode mode = AlgorithmMode::GA);

   private:
    static void print_help(const std::string& program_name, AlgorithmMode mode);
};

#endif
