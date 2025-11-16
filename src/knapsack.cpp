#include <Parser.h>
#include <helper.h>

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <numeric>
#include <span>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include "chromosome.h"
#include "io_utils.h"
#include "item.h"
#include "pop_crossover.h"
#include "pop_fit.h"
#include "pop_init.h"
#include "pop_mutation.h"
#include "pop_selection.h"
#include "rng.h"

// W razie problemu z wydajnością zamiast zwracać fitness_values można przyjąć
// referencję jako parametr by uniknać kopiowania listy.
std::tuple<std::vector<uint64_t>, uint64_t, size_t, std::vector<size_t>> population_fitness(
                                                             const std::span<Chromosome> population,
                                                             const std::span<Item> items,
                                                             const int max_weight,
                                                             const FitMethod method,
                                                             size_t elite_count) {
    const size_t pop_size = population.size();
    uint64_t best_fitness = 0;
    size_t best_index = 0;
    uint64_t total_fitness = 0;
    std::vector<uint64_t> fitness_values(population.size());

    // Zawsze zwracaj przynajmniej najlepszego osobnika
    if(elite_count < 0)
        elite_count = 1;

    // Wyliczenie przystosowania kazdego osobnika
    for (size_t i = 0; i < population.size(); ++i) {
        fitness_values[i] = fitness(items, population[i], max_weight, method);
        total_fitness += fitness_values[i];

        if (fitness_values[i] > best_fitness) {
            best_fitness = fitness_values[i];
            best_index = i;
        }
    }

    std::vector<size_t> elite_indices(elite_count);
    // Wczesny powrot jesli elitaryzm jest wylaczony
    if(elite_count < 1) {
        return {std::move(fitness_values), total_fitness, best_index, std::move(elite_indices)};
    }

    // Wyszukanie elit
    std::vector<size_t> idx(pop_size);
    std::iota(idx.begin(), idx.end(), 0);

    // Posortowanie N = {elite_count} najlepszych osobnikow
    std::ranges::partial_sort(
        idx.begin(),
        idx.begin() + elite_count,
        idx.end(),
        [&](size_t a, size_t b) {
            return fitness_values[a] > fitness_values[b]; // Malejaco
        }
    );

    for (size_t i = 0; i < elite_count; ++i)
        elite_indices[i] = idx[i];

    return {std::move(fitness_values), total_fitness, best_index, std::move(elite_indices)};
}

uint64_t algorithm(const ProgramArgs& args, const std::span<Item> items, const int max_weight,
              int output_mask = 0) {
    uint64_t best_individual_fitness = 0;
    int generations_without_improvement = 0;
    const unsigned int item_count = items.size();

    std::vector<Chromosome> population{};
    population.resize(args.pop_size);
    initialize_population(population, items, item_count, max_weight, args.initialization_method);

    // Wektor dla nowej populacji
    std::vector<Chromosome> new_population;
    new_population.resize(args.pop_size);

    // Pierwszy warunek stopu: Limit liczby generacji
    for (int generations = 0; generations < args.max_generations; ++generations) {
        if (output_mask || io_utils::PRINT_GENERATION) {
            std::cout << "Generacja " << generations << std::endl;
        }
        auto [fitness_values, total_fitness, best_index, best_indices] = population_fitness(
            population, items, max_weight, args.fit_method, args.elites);  // Wyliczenie przystosowania osobników

        const uint64_t current_best_fitness =
            fitness_values[best_index];  // Przystosowanie najlepszego osobnika z populacji

        if (output_mask) {
            io_utils::print_population_stats(output_mask, population, total_fitness,
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

        std::vector<size_t> elite_indices(args.elites);

        for (size_t i = args.elites; i < args.pop_size; i += 2) {
            Chromosome parent1 =
                select(population, fitness_values, total_fitness, args.tournament_size, args.selection_method);
            Chromosome parent2 =
                select(population, fitness_values, total_fitness, args.tournament_size, args.selection_method);

            // Krzyżowanie
            auto [children1, children2] =
                (random_float() < args.cross_chance)
                    ? crossover(parent1, parent2, args.crossover_method)
                    : std::make_pair(parent1, parent2);

            // Mutacje
            if (random_float() < args.mutation_chance)
                mutate(children1, args.mutation_method, args.mutate_per_gene);
            if (random_float() < args.mutation_chance)
                mutate(children2, args.mutation_method, args.mutate_per_gene);

            // Inwersja (jesli wlaczona)
            if(args.inversion_enabled) {
                if (random_float() < args.inversion_chance)
                    inverse(children1);
                if (random_float() < args.inversion_chance)
                    inverse(children2);
            }

            // Naprawianie (jesli wlaczone)
            if(args.repair_enabled) {
                if (random_float() < args.repair_chance)
                    repair(children1, items, max_weight);
                if (random_float() < args.repair_chance)
                    repair(children2, items, max_weight);
            }

            // Dodaj potomków do nowej populacji
            new_population[i] = children1;
            if (i + 1 < args.pop_size)
                new_population[i + 1] = children2;
        }

        for(int i = 0; i < elite_indices.size(); ++i) {
            const size_t elite_index = elite_indices[i];
            new_population[i] = population[elite_index];
        }

        // Zamiana populacji
        population.swap(new_population);
        new_population.resize(args.pop_size);
    }

    return best_individual_fitness;
}

int main(int argc, char* argv[]) {
    const ProgramArgs args = Parser::parse(argc, argv);
    int debug_mask = io_utils::PRINT_SUMMARY | io_utils::PRINT_SUMMARY | io_utils::PRINT_AVG |
                     // io_utils::PRINT_BEST_CHROM |
                     io_utils::PRINT_BEST_FITNESS;

    // Wczytanie danych
    int max_weight, optimal_value;
    auto items = io_utils::load_items(args.input_file, max_weight, optimal_value);

    // Algorytm
    const uint64_t best_fitness = algorithm(args, items, max_weight, debug_mask);

    // Wypisanie do konsoli
    const auto mutation_method =
        args.mutation_method == MutationMethod::BIT_FLIP ? "BIT_FLIP" : "MULTI_BIT_FLIP";
    const auto selection_method =
        args.selection_method == SelectionMethod::ROULETTE ? "ROULETTE" : "TOURNAMENT";

    if (debug_mask | io_utils::PRINT_SUMMARY) {
        std::cout << "==========================================================" << std::endl;
        std::cout << "POP_SIZE CROSS_CHANCE MUTATION_CHANCE MUTATION_PER_GENE_CHANCE "
                     "MAX_GENERATIONS MAX_NO_IMPROVEMENT MUTATION_METHOD SELECTION_METHOD BEST_FIT BEST_FIT_PER"
                  << std::endl;
        std::cout << args.pop_size << " " << args.cross_chance << " " << args.mutation_chance << " "
                  << args.mutate_per_gene << " " << args.max_generations << " "
                  << args.max_no_improvement << " " << mutation_method << " " << selection_method << " " << best_fitness << " "
                  << static_cast<float>(best_fitness) / optimal_value << std::endl;
        std::cout << "Najlepsze przystosowanie (wszystkie populacje): " << best_fitness
                  << std::endl;
        std::cout << "Optymalne rozwiazanie: " << to_binary_string(optimal_value, items.size()) << std::endl;
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
