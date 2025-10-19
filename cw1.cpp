#include <iostream>
#include <span>
#include <random>
#include <cmath>
#include <utility>
#include <tuple>
#include <sstream>
#include <fstream>
#include <string>
#include <filesystem>

struct Item {
  int value;
  int weight;
  Item(const int v, const int w) : value(v), weight(w) {}
};

std::string to_binary_string(const int value, const unsigned int bits) {
  std::string result;
  result.reserve(bits);
  for (int i = bits - 1; i >= 0; --i) {
    result.push_back((value & (1ULL << i)) ? '1' : '0');
  }
  return result;
}

void log_results_to_csv(const std::string& filename,
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

int random_int(const int min, const int max) {
  static std::mt19937_64 gen(std::random_device{}());
  std::uniform_int_distribution<int> dist(min, max);
  return dist(gen);
}

float random_float() {
  static std::mt19937_64 gen(std::random_device{}());
  static std::uniform_real_distribution<float> dist(0.0, 1.0);
  return dist(gen);
}

std::vector<Item> load_items(const std::string& filename, int& max_weight, int& optimal_value) {
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

void initialize_population(std::span<int> population, const unsigned int chrom_length) {
  for(int& individual : population) {
    individual = random_int(0,pow(2,chrom_length) - 1);
  }
}

void print_population(std::span<int> population, const unsigned int chrom_length) {
  for(const int& individual : population) {
    std::cout<<to_binary_string(individual, chrom_length)<<" ";
  }
  std::cout<<std::endl;
}

// Pierwszy przedmiot = najmłodszy bit (z prawej)
int fitness(const std::span<Item> items, const int chrom, const int max_weight) {
  int total_weight = 0;
  int total_value = 0;

  for (size_t i = 0; i < items.size(); ++i) {
    if (chrom & (1ULL << i)) {
      total_weight += items[i].weight;
      total_value += items[i].value;
    }
  }

  if (total_weight > max_weight) return total_value * max_weight / total_weight;
  return total_value;
}

// Punkt podziału liczony od prawej z indeksowanem od zera
std::pair<int,int> crossover(const int parent1, const int parent2, const unsigned int chrom_length) {
  const int crossover_bit = random_int(1, chrom_length-1); // n = 0 lub n=chrom_length sprawiły że dzieci byłyby jednakowe do rodziców
  const int lower_mask = (1 << crossover_bit) - 1;
  const int upper_mask = ((1 << chrom_length) - 1) ^ lower_mask;

  int child1 = (parent1 & lower_mask) | (parent2 & upper_mask);
  int child2 = (parent2 & lower_mask) | (parent1 & upper_mask);

  return {child1, child2};
}

// Punkt mutacji liczony od prawej z indeksowanem od zera
int mutate(const int chrom, const unsigned int chrom_length) {
  const int mutation_bit = random_int(0, chrom_length-1);
  const int mutation_mask = 1 << mutation_bit;

  return chrom ^ mutation_mask;
}

int roulette_select(const std::span<int> population, const std::span<const int> fitness_values, int total_fitness) {
  // Wybierz losowego osobnika jeżeli żaden nie jest przystosowany
  if (total_fitness == 0) {
    return population[random_int(0, population.size() - 1)];
  }

  // Losowanie punktu r z zakresu [0, total_fitness)
  int r = random_int(0, total_fitness - 1);

  // Znajdz osobnika która zawiera punkt r
  int cumulative = 0;
  for (size_t i = 0; i < population.size(); ++i) {
    cumulative += fitness_values[i];
    if (r < cumulative) {
      return population[i];
    }
  }

  throw std::logic_error("Funkcja metody ruletkowej nie zwróciła poprawnie osobnika");
}

// W razie problemu z wydajnością zamiast zwracać fitness_values można przyjąć referencję jako parametr by uniknać kopiowania listy.
std::tuple<std::vector<int>, int, size_t> population_fitness(const std::span<int> population, const std::span<Item> items, const int max_weight) {
  int total_fitness = 0;
  int best_fitness = 0;
  size_t best_index = 0;
  std::vector<int> fitness_values(population.size());

  for (size_t i = 0; i < population.size(); ++i) {
    fitness_values[i] = fitness(items, population[i], max_weight);
    total_fitness += fitness_values[i];
    if (fitness_values[i] > best_fitness) {
      best_fitness = fitness_values[i];
      best_index = i;
    }
  }

  return {fitness_values, total_fitness, best_index};
}

int algorithm(const int max_generations, const int pop_size, const int max_no_improvement, const float cross_chance,
  const float mutation_chance, const std::span<Item> items, const int max_weight, bool debug_print = false)
{
  int best_individual_fitness = 0;
  int generations_without_improvement = 0;
  const unsigned int item_count = items.size();
  const unsigned int chrom_length = item_count; // Alias

  std::vector<int> population{};
  population.resize(pop_size);
  initialize_population(population, item_count);

  // Wektor dla nowej populacji
  std::vector<int> new_population;
  new_population.resize(pop_size);

  // Pierwszy warunek stopu: Limit liczby generacji
  for (int generations = 0; generations < max_generations; ++generations) {
    if(debug_print) {
      std::cout<<"Generacja "<<generations<<std::endl;
    }
    auto [fitness_values, total_fitness, best_index] =
        population_fitness(population, items, max_weight); // Wyliczenie przystosowania osobników

    int current_best_fitness = fitness_values[best_index]; // Przystosowanie najlepszego osobnika z populacji

    //print_population(population, chrom_length);
    if(debug_print) {
      std::cout<<"Srednie przystosowanie nowej populacji: "<< total_fitness / pop_size << std::endl;
      //std::cout<<"Najlepszy osobnik z nowej populacji: "<< to_binary_string(population[best_index], chrom_length) << std::endl;
      std::cout<<"Najlepsze przystosowanie (wartosc plecaka) w nowej populacji: "<< current_best_fitness<<std::endl;
    }

    if (current_best_fitness > best_individual_fitness) {
      best_individual_fitness = current_best_fitness;
      generations_without_improvement = 0; // Reset
    } else {
      ++generations_without_improvement;
    }

    if (generations_without_improvement >= max_no_improvement)
      break; // Drugi warunek stopu: brak poprawy najlepszego rozwiązania

    for (size_t i = 0; i < pop_size; i += 2) {
      int parent1 = roulette_select(population, fitness_values, total_fitness);
      int parent2 = roulette_select(population, fitness_values, total_fitness);

      // Krzyżowanie jednopunktowe
      auto [children1, children2] = (random_float() < cross_chance)
                      ? crossover(parent1, parent2, chrom_length)
                      : std::make_pair(parent1, parent2);

      // Mutacje
      if (random_float() < mutation_chance) children1  = mutate(children1, chrom_length);
      if (random_float() < mutation_chance) children2 = mutate(children2, chrom_length);

      // Dodaj potomków do nowej populacji
      new_population[i] = children1;
      if (i + 1 < pop_size) new_population[i+1] = children2;
    }

    // Zamiana populacji
    population = new_population;
  }

  return best_individual_fitness;
}


int main(int argc, char* argv[]) {

  if(argc < 2) {
    std::cerr << "Uzycie: " << argv[0] << " <plik_wejsciowy> [POP_SIZE CROSS_CHANCE MUTATION_CHANCE MAX_GENERATIONS MAX_NO_IMPROVEMENT]" << std::endl;
    return 1;
  }

  std::string input_file = argv[1];

  // Domyślne wartości
  int POP_SIZE = 100;
  float CROSS_CHANCE = 0.85f;
  float MUTATION_CHANCE = 0.1f;
  int MAX_GENERATIONS = 50;
  int MAX_NO_IMPROVEMENT = 20;
  bool text_output = true;

  // Nadpisz parametry jeśli zostały podane
  if(argc > 2) POP_SIZE = std::atoi(argv[2]);
  if(argc > 3) CROSS_CHANCE = std::atof(argv[3]);
  if(argc > 4) MUTATION_CHANCE = std::atof(argv[4]);
  if(argc > 5) MAX_GENERATIONS = std::atoi(argv[5]);
  if(argc > 6) MAX_NO_IMPROVEMENT = std::atoi(argv[6]);

  // Wczytanie danych
  int max_weight, optimal_value;
  auto items = load_items(input_file, max_weight, optimal_value);

  // Algorytm
  int best_fitness = algorithm(MAX_GENERATIONS, POP_SIZE, MAX_NO_IMPROVEMENT, CROSS_CHANCE, MUTATION_CHANCE, items, max_weight, text_output);

  // Wypisanie do konsoli
  if(text_output) {
    std::cout<<"=========================================================="<<std::endl;
    std::cout<<"POP_SIZE CROSS_CHANCE MUTATION_CHANCE MAX_GENERATIONS MAX_NO_IMPROVEMENT BEST_FIT BEST_FIT_PER"<<std::endl;
    std::cout<<POP_SIZE<<" "<<CROSS_CHANCE<<" "<<MUTATION_CHANCE<<" "<<MAX_GENERATIONS<<" "<<MAX_NO_IMPROVEMENT<<" "<<best_fitness<<" "<<static_cast<float>(best_fitness) / optimal_value<<std::endl;
    std::cout<<"Najlepsze przystosowanie (wszystkie populacje): "<<best_fitness<<std::endl;
    std::cout<<"Optymalne rozwiazanie: "<<optimal_value<<std::endl;
    std::cout<<"Zblizenie do optymalnego rozwiazania: "<<static_cast<float>(best_fitness) / optimal_value<<std::endl;
  }

  // Zapis do pliku .out w formacie csv
  std::filesystem::path out_file = input_file;
  out_file.replace_extension(".csv");

  log_results_to_csv(out_file.string(),POP_SIZE,CROSS_CHANCE,MUTATION_CHANCE,MAX_GENERATIONS,MAX_NO_IMPROVEMENT,best_fitness,static_cast<float>(best_fitness) / optimal_value);

  return 0;
}

/* Źródło danych https://hjemmesider.diku.dk/~pisinger/codes.html
 * Zbiory wybrano ze względu na liczbe przedmiotów (N=50, 100, 500) oraz maksymalną wagę plecaka (C=1000,10000,25000)
 * Uwzględniono tylko jedną maksymalną wagę przedmiotów równą 1000
 * Format wejściowy dostosowano do łatwiejszego wczytywania
 *
 * Użyte zbiory:
 * knapPI_1_50_1000_1 knapPI_1_50_1000_42 knapPI_1_50_1000_85
 * knapPI_1_100_1000_1 knapPI_1_100_1000_20 knapPI_1_100_1000_52
 * knapPI_1_500_1000_1 knapPI_1_500_1000_4 knapPI_1_500_1000_10
 */