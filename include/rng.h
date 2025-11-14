#ifndef RNG_H
#define RNG_H
#include <cstdint>

int random_int(int min, int max);
uint64_t random_uint64(uint64_t min, uint64_t max);
float random_float(float min = 0.0, float max = 1.0);

#endif
