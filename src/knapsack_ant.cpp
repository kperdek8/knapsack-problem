#include <Parser.h>
#include <helper.h>

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <iostream>
#include <numeric>
#include <random>
#include <span>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include "chromosome.h"
#include "io_utils.h"
#include "item.h"
#include "rng.h"

size_t choose_index_by_probabilities(const std::vector<size_t>& candidates,
                                     const std::vector<double>& pheromone,
                                     const std::vector<double>& heuristic) {
    if (candidates.empty())
        throw std::invalid_argument("Brak kandydatow do wyboru");

    std::vector<double> weights(candidates.size());
    double total_weight = 0.0;

    // Oblicz wagę dla każdego kandydata
    for (size_t i = 0; i < candidates.size(); ++i) {
        weights[i] = pheromone[candidates[i]] * heuristic[candidates[i]];
        total_weight += weights[i];
    }

    // Losowanie proporcjonalne do wag
    double r = random_float(0, total_weight);
    double cumulative = 0.0;

    for (size_t i = 0; i < candidates.size(); ++i) {
        cumulative += weights[i];
        if (r <= cumulative)
            return candidates[i];
    }

    throw std::logic_error("Funkcja losujaca nie zwrocila poprawnie kandydata");
}

std::tuple<uint64_t, std::vector<uint64_t>> algorithm(const ProgramArgs& args, const std::span<Item> items, const int max_weight,
              const int output_mask = 0) {
    uint64_t best_individual_fitness = 0;
    int generations_without_improvement = 0;

    std::vector<uint64_t> average_fitness_history; // średnie przystosowanie w każdej generacji
    average_fitness_history.reserve(args.max_generations);

    // Wyliczenie heurystyki przedmiotow
    std::vector<double> heuristic(items.size());
    std::ranges::transform(items, heuristic.begin(), [](const Item& item){ return static_cast<float>(item.value) / static_cast<float>(item.weight); });
    // Inicjalizacja feromonow
    std::vector<double> pheromone(items.size(), args.pheromone_init);

    // Pierwszy warunek stopu: Limit liczby generacji
    for (int generation = 0; generation < args.max_generations; ++generation) {
        if (output_mask & io_utils::PRINT_GENERATION)
            std::cout << "Generacja " << generation << std::endl;
        uint64_t generation_total_fitness = 0;
        uint64_t generation_best_fitness = 0;

        std::vector<Chromosome> generation_solutions;
        std::vector<uint64_t> generation_fitnesses;

        // Iteracja po mrówkach
        for (int k = 0; k < args.pop_size; ++k) {
            Chromosome ant_solution(items.size());
            int current_weight = 0;
            int current_value = 0;

            // Wektor indeksow dostepnych przedmiotow
            std::vector<size_t> available_items(items.size());
            std::iota(available_items.begin(), available_items.end(), 0);

            while (!available_items.empty()) {
                // Filtrowanie przedmiotow ktore zmieszcza sie w plecaku
                std::vector<size_t> candidates;
                for (size_t idx : available_items) {
                    if (current_weight + items[idx].weight <= max_weight)
                        candidates.push_back(idx);
                }
                if (candidates.empty()) break;

                // Losowanie jednego przedmiotu
                std::vector<double> weights(candidates.size());
                for (size_t i = 0; i < candidates.size(); ++i) {
                    size_t item_idx = candidates[i];
                    weights[i] = std::pow(pheromone[item_idx], args.pheromone_influence) * std::pow(heuristic[item_idx], args.heuristic_influence);
                }

                // tworzymy discrete_distribution z wag
                std::discrete_distribution<size_t> dist(weights.begin(), weights.end());
                size_t chosen_candidate = dist(gen); // Indeks ze zbioru kandydatow
                size_t chosen_item = candidates[chosen_candidate]; // Indeks ze zbioru wszystkich przedmiotow

                ant_solution.set(chosen_item, true);
                current_weight += items[chosen_item].weight;
                current_value += items[chosen_item].value;

                // Usuniecie wybranego przedmiotu ze zbioru kandydatow
                std::erase(available_items, chosen_item);
            }

            // Sumowanie fitness dla generacji
            generation_total_fitness += current_value;
            // Zapisanie mrowki
            generation_solutions.push_back(ant_solution);
            generation_fitnesses.push_back(current_value);

            // Aktualizacja najlepszego rozwiązania generacji
            if (current_value > generation_best_fitness) {
                generation_best_fitness = current_value;
            }
        }

        // Średnie przystosowanie populacji
        uint64_t average_fitness = generation_total_fitness / args.pop_size;
        average_fitness_history.push_back(average_fitness);

        if (generation_best_fitness > best_individual_fitness) {
            best_individual_fitness = generation_best_fitness;
            generations_without_improvement = 0;  // Reset
        } else {
            ++generations_without_improvement;
        }

        if (generations_without_improvement >= args.max_no_improvement)
            break;  // Drugi warunek stopu: brak poprawy najlepszego rozwiązania

        // Aktualizacja feromonow
        const double best_gen_fitness = static_cast<double>(generation_best_fitness);

        for (size_t i = 0; i < items.size(); ++i) {
            // Oparowanie
            pheromone[i] *= (1.0 - args.evaporation_rate);

            // Wzmocnienie z normalizacja
            double reinforcement_sum = 0.0;

            for (int k = 0; k < args.pop_size; ++k) {
                if (generation_solutions[k][i]) {
                    double ant_fitness = static_cast<double>(generation_fitnesses[k]);
                    double relative_error = (best_gen_fitness - ant_fitness) / best_gen_fitness;
                    reinforcement_sum += args.reinforcement_constant * (1.0 / (1.0 + relative_error));
                }
            }

            pheromone[i] += reinforcement_sum;
        }

        if (output_mask) {
            io_utils::print_population_stats(output_mask, args.pop_size, generation_total_fitness, generation_best_fitness);

            if(output_mask & io_utils::WAIT_FOR_NEXT_GEN) {
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }
        }

    }

    return {best_individual_fitness, std::move(average_fitness_history)};
}

int main(int argc, char* argv[]) {
    const ProgramArgs args = Parser::parse(argc, argv, AlgorithmMode::ACO);

    int debug_mask = io_utils::PRINT_SUMMARY
        //| io_utils::WAIT_FOR_NEXT_GEN
        | io_utils::PRINT_GENERATION
        | io_utils::PRINT_AVG
        //| io_utils::PRINT_BEST_CHROM
        //| io_utils::PRINT_FINAL_BEST_CHROM
        | io_utils::PRINT_BEST_FITNESS
        | io_utils::PRINT_FINAL_BEST_FITNESS;
    if(args.debug_output == false)
        debug_mask = 0;

    // Wczytanie danych
    int max_weight, optimal_value;
    auto items = io_utils::load_items(args.input_file, max_weight, optimal_value);

    // Algorytm
    auto [best_fitness, average_fitness_history] = algorithm(args, items, max_weight, debug_mask);

    const auto population_fit_ratio = static_cast<float>(average_fitness_history.back()) / static_cast<float>(optimal_value);
    const auto best_fit_ratio = static_cast<float>(best_fitness) / static_cast<float>(optimal_value);

    if (debug_mask & io_utils::PRINT_SUMMARY) {
        std::cout << "==========================================================" << std::endl;
        std::cout << "POP_SIZE MAX_GENERATIONS MAX_NO_IMPROVEMENT PHEROMONE_INFLUENCE HEURISTIC_INFLUENCE EVAPORATION_RATE PHEROMONE_INIT REINFORCEMENT_CONSTANT BEST_FIT BEST_FIT_PER"
                  << std::endl;
        std::cout << args.pop_size << " " << args.max_generations << " " << args.max_no_improvement << " "
                  << args.pheromone_influence << " " << args.heuristic_influence << " " << args.evaporation_rate << " "
                  << args.pheromone_init << " " << args.reinforcement_constant << " " << best_fitness << " "
                  << static_cast<float>(best_fitness) / static_cast<float>(optimal_value) << std::endl;
        if(debug_mask & io_utils::PRINT_BEST_FITNESS)
            std::cout << "Najlepsze przystosowanie (wszystkie populacje): " << best_fitness << std::endl;
        if(debug_mask & io_utils::PRINT_FINAL_BEST_CHROM)
            std::cout << "Optymalne rozwiazanie: " << to_binary_string(optimal_value, items.size()) << std::endl;
        std::cout << "Zblizenie do optymalnego rozwiazania: " << best_fit_ratio << std::endl;
        std::cout << "Srednie przystosowanie ostatniej populacji: " << population_fit_ratio << std::endl;
    }

    // Zapis do pliku .out w formacie csv
    std::filesystem::path out_file = args.input_file;
    out_file.replace_extension(".csv");

    io_utils::log_results_to_csv(out_file.string(), args, best_fitness, best_fit_ratio, population_fit_ratio);

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
