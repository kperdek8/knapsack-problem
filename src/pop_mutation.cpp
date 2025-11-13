#include "pop_mutation.hpp"

#include "rng.hpp"
#include <stdexcept>

int do_bit_flip(int chrom, unsigned int chrom_length);
int do_multi_bit_flip(int chrom, unsigned int chrom_length, float per_bit_chance);

// Punkt mutacji liczony od prawej z indeksowanem od zera
// Domyslna wartosc per_bit_chance = -1.0 ma na celu wywolanie bledu podczas nie ustawienia tego parametru przy metodzie multi_bit_fip
int mutate(const int chrom, const unsigned int chrom_length, const MutationMethod method, const float per_bit_chance = -1.0) {
    switch (method) {
        case MutationMethod::BIT_FLIP:
        return do_bit_flip(chrom, chrom_length);
        case MutationMethod::MULTI_BIT_FLIP:
        return do_multi_bit_flip(chrom, chrom_length, per_bit_chance);
    }
}

int do_bit_flip(const int chrom, const unsigned int chrom_length) {
    const int mutation_bit = random_int(0, chrom_length - 1);
    const int mutation_mask = 1 << mutation_bit;

    return chrom ^ mutation_mask;
}

int do_multi_bit_flip(int chrom, const unsigned int chrom_length, const float per_bit_chance) {
    if (per_bit_chance <= 0.0f || per_bit_chance >= 1.0f)
        throw std::invalid_argument("per_bit_chance musi sie miescic w <0, 1>");

    for(int i = 0; i<chrom_length; ++i) {
        if(random_float(0.0, 1.0) <= per_bit_chance) {
            chrom ^= (1 << i);
        }
    }
    return chrom;
}