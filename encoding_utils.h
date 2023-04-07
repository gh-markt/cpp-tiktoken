#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include <stdexcept>
#define PCRE2_CODE_UNIT_WIDTH 0
#include <pcre2.h>


struct VectorHash {
    std::size_t operator()(const std::vector<uint8_t>& v) const {
        std::hash<uint8_t> hasher;
        std::size_t seed = 0;

        for (const auto& elem : v) {
            seed ^= hasher(elem) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }

        return seed;
    }
};

namespace base64 {
std::vector<uint8_t> decode(const std::string& input);
}

