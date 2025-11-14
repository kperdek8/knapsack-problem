#ifndef PARSER_H
#define PARSER_H

#include <pop_crossover.h>
#include <pop_fit.h>

#include <string>

#include "pop_init.h"
#include "pop_mutation.h"
#include "pop_selection.h"

struct ProgramArgs {
    std::string input_file;
    int pop_size = 100;
    float cross_chance = 0.85f;
    float mutation_chance = 0.1f;
    int max_generations = 50;
    int max_no_improvement = 20;
    float mutate_per_gene = 0.005f;
    int tournament_size = 5;
    InitMethod initialization_method = InitMethod::RANDOM;
    FitMethod fit_method = FitMethod::RATIO_PENALTY;
    CrossoverMethod crossover_method = CrossoverMethod::ONE_POINT;
    MutationMethod mutation_method = MutationMethod::BIT_FLIP;
    SelectionMethod selection_method = SelectionMethod::ROULETTE;
};

class Parser {
   public:
    static ProgramArgs parse(int argc, char* argv[]);

   private:
    static void print_help(const std::string& program_name);
};

#endif
