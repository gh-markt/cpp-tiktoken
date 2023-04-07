#pragma once

#include "encoding_utils.h"
#include <regex>
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>

class BytePairEncodingCore {
public:
    BytePairEncodingCore(const std::unordered_map<std::vector<uint8_t>, int, VectorHash>& byte_pair_ranks,
                         const std::unordered_map<std::string, int>& special_token_mappings,
                         const std::regex& pattern_string);

    std::pair<std::vector<int>, std::vector<int>> encode_native(const std::string& line_to_encode,
                                                                const std::unordered_set<std::string>& allowed_special);
    std::string decode_native(const std::vector<int>& input_tokens_to_decode);

private:
    std::unordered_map<std::vector<uint8_t>, int, VectorHash> byte_pair_ranks_;
    std::unordered_map<std::string, int> special_token_mappings_;
    std::regex pattern_string_;
};
