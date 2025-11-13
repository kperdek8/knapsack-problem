#include <Parser.hpp>
#include <filesystem>
#include <iostream>
#include <span>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include "io_utils.hpp"
#include "item.hpp"
#include "pop_crossover.hpp"
#include "pop_fit.hpp"
#include "pop_init.hpp"
#include "pop_mutation.hpp"
#include "pop_selection.hpp"
#include "rng.hpp"

// W razie problemu z wydajnością zamiast zwracać fitness_values można przyjąć
// referencję jako parametr by uniknać kopiowania listy.
std::tuple<std::vector<int>, int, size_t> population_fitness(const std::span<int> population,
                                                             const std::span<Item> items,
                                                             const int max_weight) {
    int total_fitness = 0;
    int best_fitness = 0;
    size_t best_index = 0;
    std::vector<int> fitness_values(population.size());

    for (size_t i = 0; i < population.size(); ++i) {
        fitness_values[i] = fitness(items, population[i], max_weight, FitMethod::RATIO_PENALTY);
        total_fitness += fitness_values[i];
        if (fitness_values[i] > best_fitness) {
            best_fitness = fitness_values[i];
            best_index = i;
        }
    }

    return {fitness_values, total_fitness, best_index};
}

int algorithm(const ProgramArgs& args, const std::span<Item> items, const int max_weight,
              int output_mask = 0) {
    int best_individual_fitness = 0;
    int generations_without_improvement = 0;
    const unsigned int item_count = items.size();
    const unsigned int chrom_length = item_count;  // Alias

    std::vector<int> population{};
    population.resize(args.pop_size);
    initialize_population(population, item_count, InitMethod::RANDOM);

    // Wektor dla nowej populacji
    std::vector<int> new_population;
    new_population.resize(args.pop_size);

    // Pierwszy warunek stopu: Limit liczby generacji
    for (int generations = 0; generations < args.max_generations; ++generations) {
        if (output_mask || io_utils::PRINT_GENERATION) {
            std::cout << "Generacja " << generations << std::endl;
        }
        auto [fitness_values, total_fitness, best_index] = population_fitness(
            population, items, max_weight);  // Wyliczenie przystosowania osobników

        int current_best_fitness =
            fitness_values[best_index];  // Przystosowanie najlepszego osobnika z
        // populacji

        if (output_mask) {
            io_utils::print_population_stats(output_mask, population, chrom_length, total_fitness,
                                             best_index, current_best_fitness);
        }

        if (current_best_fitness > best_individual_fitness) {
            best_individual_fitness = current_best_fitness;
            generations_without_improvement = 0;  // Reset
        } else {
            ++generations_without_improvement;
        }

        if (generations_without_improvement >= args.max_no_improvement)
            break;  // Drugi warunek stopu: brak poprawy najlepszego rozwiązania

        for (size_t i = 0; i < args.pop_size; i += 2) {
            int parent1 =
                select(population, fitness_values, total_fitness, SelectionMethod::ROULETTE);
            int parent2 =
                select(population, fitness_values, total_fitness, SelectionMethod::ROULETTE);

            // Krzyżowanie jednopunktowe
            auto [children1, children2] =
                (random_float() < args.cross_chance)
                    ? crossover(parent1, parent2, chrom_length, CrossoverMethod::ONE_POINT)
                    : std::make_pair(parent1, parent2);

            // Mutacje
            if (random_float() < args.mutation_chance)
                children1 =
                    mutate(children1, chrom_length, args.mutation_method, args.mutate_per_gene);
            if (random_float() < args.mutation_chance)
                children2 =
                    mutate(children2, chrom_length, args.mutation_method, args.mutate_per_gene);

            // Dodaj potomków do nowej populacji
            new_population[i] = children1;
            if (i + 1 < args.pop_size)
                new_population[i + 1] = children2;
        }

        // Zamiana populacji
        population = new_population;
    }

    return best_individual_fitness;
}

int main(int argc, char* argv[]) {
    ProgramArgs args = Parser::parse(argc, argv);
    int debug_mask = io_utils::PRINT_SUMMARY | io_utils::PRINT_SUMMARY | io_utils::PRINT_AVG |
                     // io_utils::PRINT_BEST_CHROM |
                     io_utils::PRINT_BEST_FITNESS;

    // Wczytanie danych
    int max_weight, optimal_value;
    auto items = io_utils::load_items(args.input_file, max_weight, optimal_value);

    // Algorytm
    int best_fitness = algorithm(args, items, max_weight, debug_mask);

    // Wypisanie do konsoli
    const auto mutation_method =
        args.mutation_method == MutationMethod::BIT_FLIP ? "BIT_FLIP" : "MULTI_BIT_FLIP";

    if (debug_mask | io_utils::PRINT_SUMMARY) {
        std::cout << "==========================================================" << std::endl;
        std::cout << "POP_SIZE CROSS_CHANCE MUTATION_CHANCE MUTATION_PER_GENE_CHANCE "
                     "MAX_GENERATIONS MAX_NO_IMPROVEMENT BEST_FIT BEST_FIT_PER"
                  << std::endl;
        std::cout << args.pop_size << " " << args.cross_chance << " " << args.mutation_chance << " "
                  << args.mutate_per_gene << " " << args.max_generations << " "
                  << args.max_no_improvement << " " << mutation_method << " " << best_fitness << " "
                  << static_cast<float>(best_fitness) / optimal_value << std::endl;
        std::cout << "Najlepsze przystosowanie (wszystkie populacje): " << best_fitness
                  << std::endl;
        std::cout << "Optymalne rozwiazanie: " << optimal_value << std::endl;
        std::cout << "Zblizenie do optymalnego rozwiazania: "
                  << static_cast<float>(best_fitness) / optimal_value << std::endl;
    }

    // Zapis do pliku .out w formacie csv
    std::filesystem::path out_file = args.input_file;
    out_file.replace_extension(".csv");

    io_utils::log_results_to_csv(out_file.string(), args, best_fitness,
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
