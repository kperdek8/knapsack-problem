#include "pop_crossover.h"

#include <stdexcept>

#include "rng.h"

std::pair<Chromosome, Chromosome> do_one_point(const Chromosome& parent1,
                                               const Chromosome& parent2);
std::pair<Chromosome, Chromosome> do_two_point(const Chromosome& parent1,
                                               const Chromosome& parent2);
std::pair<Chromosome, Chromosome> do_uniform(const Chromosome& parent1, const Chromosome& parent2);

std::pair<Chromosome, Chromosome> crossover(const Chromosome& parent1, const Chromosome& parent2,
                                            const CrossoverMethod method) {
    switch (method) {
        case CrossoverMethod::ONE_POINT:
            return do_one_point(parent1, parent2);
        case CrossoverMethod::TWO_POINT:
            return do_two_point(parent1, parent2);
        case CrossoverMethod::UNIFORM:
            throw std::logic_error("Metoda krzyzowania UNIFORM nie jest zaimplementowana");
    }
    throw std::logic_error("Do krzyzowania zostala przekazana nieznana metoda");
}

// Punkt podziału liczony od prawej z indeksowanem od zera
std::pair<Chromosome, Chromosome> do_one_point(const Chromosome& parent1,
                                               const Chromosome& parent2) {
    auto chrom_length = parent1.size();

    // n = 0 lub n=chrom_length sprawiły że dzieci byłyby jednakowe do rodziców
    const int crossover_bit = random_int(1, chrom_length - 1);

    Chromosome lower_mask(chrom_length);
    Chromosome upper_mask(chrom_length);
    for (size_t i = 0; i < chrom_length; ++i) {
        if (i < crossover_bit)
            lower_mask.set(i, true);
        else
            upper_mask.set(i, true);
    }

    Chromosome child1 = (parent1 & lower_mask) | (parent2 & upper_mask);
    Chromosome child2 = (parent2 & lower_mask) | (parent1 & upper_mask);

    return {child1, child2};
}

// Punkt podziału liczony od prawej z indeksowanem od zera
std::pair<Chromosome, Chromosome> do_two_point(const Chromosome& parent1,
                                               const Chromosome& parent2) {
    if (parent1.size() != parent2.size())
        throw std::logic_error("Krzyzowaniu poddano rodzicow o roznej dlugosci chromosomu");
    auto chrom_length = parent1.size();

    // n = 0 lub n=chrom_length sprawiły że dzieci byłyby jednakowe do rodziców
    const int crossover_left_bit = random_int(1, chrom_length - 2);
    const int crossover_right_bit = random_int(crossover_left_bit + 1, chrom_length - 1);

    Chromosome inner_mask(chrom_length);
    Chromosome outer_mask(chrom_length);
    for (size_t i = 0; i < chrom_length; ++i) {
        if (i < crossover_left_bit || i > crossover_right_bit)
            outer_mask.set(i, true);
        else
            inner_mask.set(i, true);
    }

    Chromosome child1 = (parent1 & outer_mask) | (parent2 & inner_mask);
    Chromosome child2 = (parent2 & outer_mask) | (parent1 & inner_mask);

    return {child1, child2};
}
