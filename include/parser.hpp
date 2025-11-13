#ifndef PARSER_H
#define PARSER_H

#include "pop_mutation.hpp"
#include <string>
#include <iostream>

struct ProgramArgs {
  std::string input_file;
  int pop_size = 100;
  float cross_chance = 0.85f;
  float mutation_chance = 0.1f;
  int max_generations = 50;
  int max_no_improvement = 20;
  float mutate_per_gene = 0.005f;
  MutationMethod mutation_method = MutationMethod::BIT_FLIP;
};

class Parser {
  public:
    static ProgramArgs parse(int argc, char* argv[]);

  private:
    static void print_help(const std::string& program_name);
};



#endif
