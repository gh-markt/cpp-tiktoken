#include "modelparams.h"
#include "emdedded_resource_reader.h"
#include <stdexcept>
#include <optional>

// ModelParams constructor and member functions

ModelParams::ModelParams(int explicit_n_vocab,
                         const std::string& pat_str,
                         const std::unordered_map<std::vector<uint8_t>, int, VectorHash>& mergeable_ranks,
                         const std::unordered_map<std::string, int>& special_tokens)
        : explicit_n_vocab_(explicit_n_vocab),
          pat_str_(pat_str),
          mergeable_ranks_(mergeable_ranks),
          special_tokens_(special_tokens) {}

int ModelParams::explicit_n_vocab() const { return explicit_n_vocab_; }

std::string ModelParams::pat_str() const { return pat_str_; }

std::unordered_map<std::vector<uint8_t>, int, VectorHash> ModelParams::mergeable_ranks() const {
    return mergeable_ranks_;
}

std::unordered_map<std::string, int> ModelParams::special_tokens() const { return special_tokens_; }

// ModelParamsGenerator member functions

ModelParams ModelParamsGenerator::get_model_params(LanguageModel model) {
    switch(model) {
        case LanguageModel::CL100K_BASE:
            return cl100k_base();
        case LanguageModel::P50K_BASE:
            return p50k_base();
        case LanguageModel::P50K_EDIT:
            return p50k_edit();
        case LanguageModel::R50K_BASE:
            return r50k_base();
    }
    throw std::runtime_error("Invalid argument to get_model_params");
}

#if 1
// latin alphabet only
static auto constexpr cl100k_pattern = "(?:'s|'t|'re|'ve|'m|'ll|'d)|[^\\r\\na-zA-Z0-9]?[a-zA-Z]+|[0-9]{1,3}| ?[^\\sa-zA-Z0-9]+[\\r\\n]*|\\s*[\\r\\n]+|\\s+(?!\\S)|\\s+";
static auto constexpr p50k_pattern = "'s|'t|'re|'ve|'m|'ll|'d| ?[a-zA-Z]+| ?[0-9]+| ?[^\\sa-zA-Z0-9]+|\\s+(?!\\S)|\\s+";
#else
// original regexes don't work with std::regex.  Need to come up with another solution to support other languages.
static auto constexpr cl100k_pattern = "(?i:'s|'t|'re|'ve|'m|'ll|'d)|[^\\r\\n\\p{L}\\p{N}]?\\p{L}+|\\p{N}{1,3}| ?[^\\s\\p{L}\\p{N}]+[\\r\\n]*|\\s*[\\r\\n]+|\\s+(?!\\S)|\\s+";
static auto constexpr p50k_pattern = "(?i:'s|'t|'re|'ve|'m|'ll|'d)| ?\\p{L}+| ?\\p{N}+| ?[^\\s\\p{L}\\p{N}]+|\\s+(?!\\S)|\\s+";
#endif
ModelParams ModelParamsGenerator::r50k_base() {
    auto mergeableRanks = EmbeddedResourceReader::loadTokenBytePairEncoding("r50k_base.tiktoken");

    return ModelParams(
            50257,
            p50k_pattern,
            mergeableRanks,
            {{EndOfText, 50256}}
    );
}

ModelParams ModelParamsGenerator::p50k_base() {
    auto mergeableRanks = EmbeddedResourceReader::loadTokenBytePairEncoding("p50k_base.tiktoken");

    return ModelParams(
            50281,
            p50k_pattern,
            mergeableRanks,
            {{EndOfText, 50256}}
    );
}

ModelParams ModelParamsGenerator::p50k_edit() {
    auto mergeableRanks = EmbeddedResourceReader::loadTokenBytePairEncoding("p50k_base.tiktoken");

    std::unordered_map<std::string, int> specialTokens = {
            {EndOfText, 50256},
            {FimPrefix, 50281},
            {FimMiddle, 50282},
            {FimSuffix, 50283}
    };

    return ModelParams(
            0,
            p50k_pattern,
            mergeableRanks,
            specialTokens
    );
}

ModelParams ModelParamsGenerator::cl100k_base() {
    auto mergeableRanks = EmbeddedResourceReader::loadTokenBytePairEncoding("cl100k_base.tiktoken");
    std::unordered_map<std::string, int> specialTokens = {
            {EndOfText, 100257},
            {FimPrefix, 100258},
            {FimMiddle, 100259},
            {FimSuffix, 100260},
            {EndOfPrompt, 100276}
    };

    return ModelParams(
            0,
            cl100k_pattern,
            mergeableRanks,
            specialTokens
    );
}
