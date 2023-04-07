//
// Created by markt on 4/5/23.
//

#include "encoding.h"
#include "modelparams.h"

#include <stdexcept>


GptEncoding::GptEncoding(const std::string& pattern_string,
                         const std::unordered_map<std::vector<uint8_t>, int, VectorHash>& byte_pair_ranks,
                         const std::unordered_map<std::string, int>& special_token_mappings,
                         int explicit_n_vocab)
        : max_token_value_(0),
          special_token_mappings_(special_token_mappings),
          byte_pair_encoding_core_processor_(byte_pair_ranks, special_token_mappings, std::regex(pattern_string, std::regex_constants::extended | std::regex_constants::icase)) {
    // Implementation of the constructor
}

std::shared_ptr<GptEncoding> GptEncoding::get_encoding(LanguageModel model) {
    ModelParams model_params = ModelParamsGenerator::get_model_params(model);
    return std::make_shared<GptEncoding>(model_params.pat_str(), model_params.mergeable_ranks(), model_params.special_tokens(),
                       model_params.explicit_n_vocab());
}


std::vector<int> GptEncoding::encode(const std::string& line_to_encode,
                                     const std::unordered_set<std::string>& allowed_special,
                                     const std::unordered_set<std::string>& disallowed_special) {
    // Check for disallowed special tokens
    if (disallowed_special.count("all") > 0) {
        for (const auto& special_token : special_token_mappings_) {
            if (line_to_encode.find(special_token.first) != std::string::npos) {
                throw std::invalid_argument("Disallowed special token found: " + special_token.first);
            }
        }
    }

    // Call the encode_native function from the BytePairEncodingCore class
    return byte_pair_encoding_core_processor_.encode_native(line_to_encode, allowed_special).first;
}

std::string GptEncoding::decode(const std::vector<int>& input_tokens_to_decode) {
    // Call the decode_native function from the BytePairEncodingCore class
    return byte_pair_encoding_core_processor_.decode_native(input_tokens_to_decode);
}