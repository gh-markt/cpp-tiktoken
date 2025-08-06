/*
 * Copyright (c) 2023 by Mark Tarrabain All rights reserved. Redistribution and use in source and binary forms,
 * with or without modification, are permitted provided that the following conditions are met:
 * Redistributions of source code must retain the above copyright notice, this list of conditions and the following
 * disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following
 * disclaimer in the documentation and/or other materials provided with the distribution.
 * Neither the name of the nor the names of its contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "encoding.h"
#include "modelparams.h"
#include "pcre2_regex.h"

#include <stdexcept>
#define PCRE2_CODE_UNIT_WIDTH 0
#include <pcre2.h>

GptEncoding::GptEncoding(std::string&& pattern_string, bpe_encoding_t&& byte_pair_ranks,
    std::unordered_map<std::string, int>&& special_token_mappings, int explicit_n_vocab) :
    n_words(explicit_n_vocab),
    byte_pair_encoding_core_processor_(std::move(byte_pair_ranks), std::move(special_token_mappings),
        PCRERegex(pattern_string, PCRE2_CASELESS)) { }

GptEncoding GptEncoding::get_encoding(ModelParams &&params)
{
    return GptEncoding(std::move(params.pat_str), std::move(params.mergeable_ranks),
        std::move(params.special_tokens), params.explicit_n_vocab);
}

GptEncoding GptEncoding::get_encoding_llama3(ModelParams &&params)
{
    const int num_reserved_special_tokens = 256;
    const int num_base_tokens = (int) params.mergeable_ranks.size();

    std::unordered_map<std::string, int> special_llama3_token_mappings;
    std::vector<std::string> list_special_tokens = { "<|begin_of_text|>",
        "<|end_of_text|>",
        "<|reserved_special_token_0|>",
        "<|reserved_special_token_1|>",
        "<|reserved_special_token_2|>",
        "<|reserved_special_token_3|>",
        "<|start_header_id|>",
        "<|end_header_id|>",
        "<|reserved_special_token_4|>",
        "<|eot_id|>" };
    for (int i = 5; i < num_reserved_special_tokens - 5; i++) {
        list_special_tokens.push_back("<|reserved_special_token_" + std::to_string(i) + "|>");
    }
    for (int i = 0; i < list_special_tokens.size(); i++) {
        special_llama3_token_mappings.insert({ list_special_tokens[i], num_base_tokens + i });
    }

    return GptEncoding(std::move(params.pat_str), std::move(params.mergeable_ranks),
        std::move(special_llama3_token_mappings), params.explicit_n_vocab);
}

GptEncoding GptEncoding::get_encoding_llama3_1(ModelParams &&params)
{
    const int num_reserved_special_tokens = 256;
    const int num_base_tokens = (int) params.mergeable_ranks.size();

    std::unordered_map<std::string, int> special_llama3_token_mappings;
    std::vector<std::string> list_special_tokens = { "<|begin_of_text|>",
        "<|end_of_text|>",
        "<|reserved_special_token_0|>",
        "<|reserved_special_token_1|>",
        "<|finetune_right_pad_id|>",
        "<|reserved_special_token_2|>",
        "<|start_header_id|>",
        "<|end_header_id|>",
        "<|eom_id|>",
        "<|eot_id|>",
        "<|python_tag|>" };
    for (int i = 5; i < num_reserved_special_tokens - 3; i++) {
        list_special_tokens.push_back("<|reserved_special_token_" + std::to_string(i) + "|>");
    }
    for (int i = 0; i < list_special_tokens.size(); i++) {
        special_llama3_token_mappings.insert({ list_special_tokens[i], num_base_tokens + i });
    }

    return GptEncoding(std::move(params.pat_str), std::move(params.mergeable_ranks),
        std::move(special_llama3_token_mappings), params.explicit_n_vocab);
}

GptEncoding GptEncoding::get_encoding(LanguageModel model, IResourceReader *resource_reader, const char *resource_name)
{
    ModelParams model_params = ModelParamsGenerator::get_model_params(model, resource_name, resource_reader);

    return get_encoding(std::move(model_params));
}

// Implement with custom additional params for LLAMA 3

GptEncoding GptEncoding::get_encoding_llama3(LanguageModel model, IResourceReader *resource_reader, const char *resource_name)
{
    ModelParams model_params = ModelParamsGenerator::get_model_params(model, resource_name, resource_reader);

    return get_encoding_llama3(std::move(model_params));
}

// Implement with custom additional params for LLAMA 3.1 LLAMA 3.2

GptEncoding GptEncoding::get_encoding_llama3_1(LanguageModel model, IResourceReader *resource_reader, const char *resource_name)
{
    ModelParams model_params = ModelParamsGenerator::get_model_params(model, resource_name, resource_reader);

    return get_encoding_llama3_1(std::move(model_params));
}

std::vector<int> GptEncoding::encode(const std::string &line_to_encode, const std::unordered_set<std::string> &allowed_special,
    const std::unordered_set<std::string> &disallowed_special)
{
    // Check for disallowed special tokens
    if (disallowed_special.count("all") > 0) {
        for (const auto &special_token: byte_pair_encoding_core_processor_.getSpecialTokenMappings()) {
            if (line_to_encode.find(special_token.first) != std::string::npos) {
#if TIKTOKEN_EXCEPTIONS_ENABLE
                throw std::invalid_argument("Disallowed special token found: " + special_token.first);
#else
                return {};
#endif
            }
        }
    }
    // Call the encode_native function from the BytePairEncodingCore class
    return byte_pair_encoding_core_processor_.encode_native(line_to_encode, allowed_special).first;
}

std::string GptEncoding::decode(const std::vector<int> &input_tokens_to_decode)
{
    // Call the decode_native function from the BytePairEncodingCore class
    return byte_pair_encoding_core_processor_.decode_native(input_tokens_to_decode);
}

const bpe_encoding_t &GptEncoding::get_byte_pair_token_map() const
{
    return byte_pair_encoding_core_processor_.getBytePairRanks();
}
