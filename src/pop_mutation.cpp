#include "pop_mutation.h"

#include <item.h>

#include <iostream>
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

void inverse(Chromosome& chrom) {
    const int offset = random_int(0, chrom.size() - 2);
    const int end = random_int(offset + 1, chrom.size() - 1);

    int left = offset;
    int right = end;

    while(left < right) {
        const bool tmp = chrom[left];
        chrom.set(left, chrom[right]);
        chrom.set(right, tmp);

        left++;
        right--;
    }
}

void repair(Chromosome& chrom, const std::span<Item> items, const int max_weight) {
    int total_weight = 0;

    std::cout<<"Naprawianie chromosomu"<<std::endl;

    // Obliczenie wagi zakodowanch przedmiotow
    for (size_t i = 0; i < items.size(); ++i) {
        if (chrom[i]) {
            total_weight += items[i].weight;
        }
    }

    // Waga zakodowanych przedmiotow miesci sie w limicie, nie trzeba naprawiac
    if(total_weight <= max_weight) {
        return;
    }

    // Zerowanie kolejnych genow dopóki waga nie spadnie poniżej limitu
    for(int i = 0; i < chrom.size(); ++i) {
        if(chrom[i] == true) {
            chrom.set(i, false);
            total_weight -= items[i].weight;
            if (total_weight <= max_weight)
                return;
        }
    }
}