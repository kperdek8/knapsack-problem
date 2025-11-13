#include "rng.hpp"

#include <random>

int random_int(const int min, const int max) {
    static std::mt19937_64 gen(std::random_device{}());
    std::uniform_int_distribution<int> dist(min, max);
    return dist(gen);
}

float random_float(const float min, const float max) {
    static std::mt19937_64 gen(std::random_device{}());
    std::uniform_real_distribution<float> dist(min, max);
    return dist(gen);
}
