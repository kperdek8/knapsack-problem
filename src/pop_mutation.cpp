#include "pop_mutation.hpp"
#include "rng.hpp"
#include <stdexcept>

int do_bit_flip(const int chrom, const unsigned int chrom_length);
int do_multi_bit_flip(const int chrom, const unsigned int chrom_length);

// Punkt mutacji liczony od prawej z indeksowanem od zera
int mutate(const int chrom, const unsigned int chrom_length, MutationMethod method) {
    switch (method) {
        case MutationMethod::BIT_FLIP:
        return do_bit_flip(chrom, chrom_length);
        case MutationMethod::MULTI_BIT_FLIP:
        return do_multi_bit_flip(chrom, chrom_length);
    }
}

int do_bit_flip(const int chrom, const unsigned int chrom_length) {
    const int mutation_bit = random_int(0, chrom_length - 1);
    const int mutation_mask = 1 << mutation_bit;

    return chrom ^ mutation_mask;
}

int do_multi_bit_flip(const int chrom, const unsigned int chrom_length) {
    throw std::logic_error("Metoda mutacji MULTI_BIT_FLIP nie jest jeszcze zaimplementowana");
}
