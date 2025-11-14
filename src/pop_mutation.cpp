#include "pop_mutation.h"

#include <stdexcept>

#include "rng.h"

void do_bit_flip(Chromosome& chrom);
void do_multi_bit_flip(Chromosome& chrom,  float per_bit_chance);

void mutate(Chromosome& chrom, const MutationMethod method,
           const float per_bit_chance) {
    switch (method) {
        case MutationMethod::BIT_FLIP:
            do_bit_flip(chrom);
            break;
        case MutationMethod::MULTI_BIT_FLIP:
            do_multi_bit_flip(chrom, per_bit_chance);
            break;
        default:
            throw std::logic_error("Do mutacji zostala przekazana nieznana metoda");
    }
}

// Punkt mutacji liczony od prawej z indeksowanem od zera
void do_bit_flip(Chromosome& chrom) {
    const int mutation_bit = random_int(0, chrom.size() - 1);
    chrom.flip(mutation_bit);
}

void do_multi_bit_flip(Chromosome& chrom, const float per_bit_chance) {
    if (per_bit_chance <= 0.0f || per_bit_chance >= 1.0f)
        throw std::invalid_argument("per_bit_chance musi sie miescic w <0, 1>");

    for (int i = 0; i < chrom.size(); ++i) {
        if (random_float(0.0, 1.0) <= per_bit_chance) {
            chrom.flip(i);
        }
    }
}
