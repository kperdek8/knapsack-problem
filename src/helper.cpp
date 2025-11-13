#include "helper.hpp"

std::string to_binary_string(const int value, const unsigned int bits) {
    std::string result;
    result.reserve(bits);
    for (int i = bits - 1; i >= 0; --i) {
        result.push_back((value & (1ULL << i)) ? '1' : '0');
    }
    return result;
}
