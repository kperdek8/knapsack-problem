#include "Parser.hpp"

ProgramArgs Parser::parse(int argc, char* argv[]) {
    ProgramArgs args;

    if (argc < 2) {
        print_help(argv[0]);
        std::exit(1);
    }

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "--input" && i + 1 < argc)
            args.input_file = argv[++i];
        else if (arg == "--pop" && i + 1 < argc)
            args.pop_size = std::atoi(argv[++i]);
        else if (arg == "--cross" && i + 1 < argc)
            args.cross_chance = std::atof(argv[++i]);
        else if (arg == "--mut" && i + 1 < argc)
            args.mutation_chance = std::atof(argv[++i]);
        else if (arg == "--gen" && i + 1 < argc)
            args.max_generations = std::atoi(argv[++i]);
        else if (arg == "--no_improve" && i + 1 < argc)
            args.max_no_improvement = std::atoi(argv[++i]);
        else if (arg == "--mutate" && i + 1 < argc) {
            std::string method = argv[++i];
            if (method == "single")
                args.mutation_method = MutationMethod::BIT_FLIP;
            else if (method == "multi")
                args.mutation_method = MutationMethod::MULTI_BIT_FLIP;
            else {
                std::cerr << "Nieznana metoda mutacji: " << method << "\n";
                std::exit(1);
            }
        } else if (arg == "--mut_per_gene" && i + 1 < argc)
            args.mutate_per_gene = std::atof(argv[++i]);
        else if (arg == "--help" || arg == "-h") {
            print_help(argv[0]);
            std::exit(0);
        } else {
            std::cerr << "Nieznana flaga: " << arg << "\n";
            print_help(argv[0]);
            std::exit(1);
        }
    }

    if (args.input_file.empty()) {
        std::cerr << "Brak pliku wejściowego (--input <plik>)\n";
        std::exit(1);
    }

    return args;
}

void Parser::print_help(const std::string& program_name) {
    std::cerr << "Użycie:\n"
              << "  " << program_name << " --input <plik_wejsciowy> [opcje]\n\n"
              << "Opcje:\n"
              << "  --pop <rozmiar_populacji> \n  (domyslnie 100)\n"
              << "  --cross <prawdopodobienstwo_krzyzowania> \n  (domyslnie 0.85)\n"
              << "  --mut <prawdopodobienstwo_mutacji> \n  (domyslnie 0.1)\n"
              << "  --mut_per_gene <prawdopodobienstwo_mutacji> | Wymaga metody --mutate multi\n  "
                 "(domyslnie 0.005)\n"
              << "  --gen <liczba_generacji> \n  (domyslnie 50)\n"
              << "  --no_improve <maks_brak_poprawy> \n  (domyslnie 20)\n"
              << "  --mutate single|multi \n  (domyslnie single)\n"
              << "  --help (wyswietla ten komunikat)  \n"
              << std::endl;
}
