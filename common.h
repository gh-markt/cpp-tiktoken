#pragma once

#define TIKTOKEN_EXCEPTIONS_ENABLE 1

#include <unordered_map>
#include <vector>
#include <string_view>

#include "encoding_utils.h"

struct bpe_encoding_hash_t
{
    std::size_t operator()(const std::vector<uint8_t>& v) const
    {
        const char* const begin = reinterpret_cast<const char*>(v.data());
        return std::hash<std::string_view> {} (std::string_view(begin, v.size()));
    }
};
using bpe_encoding_t = std::unordered_map<std::vector<uint8_t>, int, bpe_encoding_hash_t>;