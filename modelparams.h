#pragma once

#include "encoding_utils.h"
#include <string>
#include <unordered_map>
#include <vector>

class ModelParams {
public:
    ModelParams(int explicit_n_vocab,
                const std::string& pat_str,
                const std::unordered_map<std::vector<uint8_t>, int, VectorHash>& mergeable_ranks,
                const std::unordered_map<std::string, int>& special_tokens);

    int explicit_n_vocab() const;
    std::string pat_str() const;
    std::unordered_map<std::vector<uint8_t>, int, VectorHash> mergeable_ranks() const;
    std::unordered_map<std::string, int> special_tokens() const;

private:
    int explicit_n_vocab_;
    std::string pat_str_;
    std::unordered_map<std::vector<uint8_t>, int, VectorHash> mergeable_ranks_;
    std::unordered_map<std::string, int> special_tokens_;
};

enum class LanguageModel {
    CL100K_BASE,
    R50K_BASE,
    P50K_BASE,
    P50K_EDIT
};

class ModelParamsGenerator {
public:
    static ModelParams get_model_params(LanguageModel model);
    static auto constexpr EndOfText = "<|endoftext|>";
    static auto constexpr FimPrefix = "<|fim_prefix|>";
    static auto constexpr FimMiddle = "<|fim_middle|>";
    static auto constexpr FimSuffix = "<|fim_suffix|>";
    static auto constexpr EndOfPrompt = "<|endofprompt|>";

private:
    static ModelParams r50k_base();
    static ModelParams p50k_base();
    static ModelParams p50k_edit();
    static ModelParams cl100k_base();
};
