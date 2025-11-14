#ifndef CHROMOSOME_H
#define CHROMOSOME_H

#include <cstdint>
#include <ranges>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include "rng.h"

class Chromosome {
public:
    explicit Chromosome() : genes(0) {}
    explicit Chromosome(const size_t size) : genes(size, false) {}

    static Chromosome random(unsigned int size) {
        Chromosome c(size);
        unsigned int full_blocks = size / 64;
        unsigned int remaining_bits = size % 64;

        for (unsigned int b = 0; b < full_blocks; ++b) {
            uint64_t val = random_uint64(0, UINT64_MAX);
            for (unsigned int i = 0; i < 64; ++i)
                c.set(b * 64 + i, (val >> i) & 1);
        }

        if (remaining_bits > 0) {
            uint64_t val = random_uint64(0, UINT64_MAX);
            for (unsigned int i = 0; i < remaining_bits; ++i)
                c.set(full_blocks * 64 + i, (val >> i) & 1);
        }

        return c;
    }

    // Operacje bitowe
    Chromosome operator|(const Chromosome& other) const {
        check_size(other);
        Chromosome result(genes.size());
        for (size_t i = 0; i < genes.size(); ++i)
            result.genes[i] = genes[i] | other.genes[i];
        return result;
    }

    Chromosome operator&(const Chromosome& other) const {
        check_size(other);
        Chromosome result(genes.size());
        for (size_t i = 0; i < genes.size(); ++i)
            result.genes[i] = genes[i] & other.genes[i];
        return result;
    }

    Chromosome operator^(const Chromosome& other) const {
        check_size(other);
        Chromosome result(genes.size());
        for (size_t i = 0; i < genes.size(); ++i)
            result.genes[i] = genes[i] ^ other.genes[i];
        return result;
    }

    void flip(size_t idx) {
        if (idx >= genes.size()) throw std::out_of_range("Bit index out of range");
        genes[idx] = !genes[idx];
    }

    void set(size_t idx, bool value) {
        if (idx >= genes.size()) throw std::out_of_range("Bit index out of range");
        genes[idx] = value;
    }

    // Odczyt bitu
    bool operator[](size_t idx) const {
        if (idx >= genes.size()) throw std::out_of_range("Bit index out of range");
        return genes[idx];
    }

    size_t size() const { return genes.size(); }

    // Konwersja string
    [[nodiscard]] std::string to_string() const {
        std::ostringstream oss;
        for (bool gene : std::ranges::reverse_view(genes))
            oss << (gene ? '1' : '0');
        return oss.str();
    }

private:
    std::vector<bool> genes;

    void check_size(const Chromosome& other) const {
        if (genes.size() != other.genes.size())
            throw std::logic_error("Chromosome sizes do not match");
    }
};

#endif //CHROMOSOME_H
