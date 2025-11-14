#include "rng.h"

#include <random>


static std::mt19937_64 gen(std::random_device{}());

int random_int(const int min, const int max) {
    std::uniform_int_distribution<int> dist(min, max);
    return dist(gen);
}

uint64_t random_uint64(const uint64_t min, const uint64_t max) {
    std::uniform_int_distribution<uint64_t> dist(min, max);
    return dist(gen);
}

float random_float(const float min, const float max) {
    std::uniform_real_distribution<float> dist(min, max);
    return dist(gen);
}
