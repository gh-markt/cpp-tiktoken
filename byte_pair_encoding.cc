//
// Created by markt on 4/5/23.
//

#include "byte_pair_encoding.h"
#include <string>
#include <algorithm>

BytePairEncodingCore::BytePairEncodingCore(const std::unordered_map<std::vector<uint8_t>, int, VectorHash>& byte_pair_ranks,
                                           const std::unordered_map<std::string, int>& special_token_mappings,
                                           const std::regex& pattern_string)
        : byte_pair_ranks_(byte_pair_ranks), special_token_mappings_(special_token_mappings), pattern_string_(pattern_string) {}

std::pair<std::vector<int>, std::vector<int>> BytePairEncodingCore::encode_native(const std::string& line_to_encode,
                                                                                  const std::unordered_set<std::string>& allowed_special) {
    std::vector<int> tokens;
    std::vector<int> segment_ids;

    for (std::sregex_iterator it = std::sregex_iterator(line_to_encode.begin(), line_to_encode.end(), pattern_string_),
                 end = std::sregex_iterator();
         it != end; ++it) {
        std::string token = it->str();

        auto special_mapping = special_token_mappings_.find(token);
        if (special_mapping != special_token_mappings_.end() && allowed_special.count(token) > 0) {
            tokens.push_back(special_mapping->second);
            segment_ids.push_back(0);
        } else {
            std::vector<uint8_t> utf8_encoded(token.begin(), token.end());
            int prev_token_start = 0;

            for (size_t i = 0; i < utf8_encoded.size();) {
                int token_id = utf8_encoded[i];
                int token_length = 1;
                auto current_token = std::string(utf8_encoded.begin() + prev_token_start, utf8_encoded.begin() + prev_token_start + token_length);
                auto lookup = std::vector<uint8_t>(current_token.begin(), current_token.end());

                while (true) {
                    auto byte_pair_rank = byte_pair_ranks_.find(lookup);

                    if (byte_pair_rank == byte_pair_ranks_.end()) {
                        --token_length;
                        break;
                    }

                    token_id = byte_pair_rank->second;
                    ++token_length;
                    if (prev_token_start + token_length > utf8_encoded.size()) {
                        --token_length;
                        break;
                    }
                    current_token = std::string(utf8_encoded.begin() + prev_token_start, utf8_encoded.begin() + prev_token_start + token_length);
                    lookup = std::vector<uint8_t>(current_token.begin(), current_token.end());
                }

                tokens.push_back(token_id);
                segment_ids.push_back(0);
                i += token_length;
                prev_token_start = i;
            }
        }
    }

    return std::make_pair(tokens, segment_ids);
}

std::string BytePairEncodingCore::decode_native(const std::vector<int>& input_tokens_to_decode) {
    std::stringstream decoded_string;

    for (const int token_id : input_tokens_to_decode) {
        auto special_token = std::find_if(special_token_mappings_.begin(), special_token_mappings_.end(),
                                          [token_id](const auto& pair) { return pair.second == token_id; });

        if (special_token != special_token_mappings_.end()) {
            decoded_string << special_token->first;
        } else {
            for (const auto& byte_pair : byte_pair_ranks_) {
                if (byte_pair.second == token_id) {
                    decoded_string << std::string(byte_pair.first.begin(), byte_pair.first.end());
                    break;
                }
            }
        }
    }

    return decoded_string.str();
}
