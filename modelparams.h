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
#pragma once

#include "common.h"
#include "encoding_utils.h"
#include <string>
#include <unordered_map>
#include <vector>

class ModelParams {
public:
    ModelParams();
    ModelParams(int explicit_n_vocab, std::string&& pat_str,
        bpe_encoding_t&& mergeable_ranks,
        std::unordered_map<std::string, int>&& special_tokens);

    int explicit_n_vocab;
    std::string pat_str;
    bpe_encoding_t mergeable_ranks;
    std::unordered_map<std::string, int> special_tokens;
};

enum class LanguageModel {
    O200K_BASE,
    CL100K_BASE,
    R50K_BASE,
    P50K_BASE,
    P50K_EDIT,

    COUNT
};

class IResourceReader;

class ModelParamsGenerator {
public:
    static ModelParams get_model_params(LanguageModel model, const char* resource_name = nullptr, IResourceReader* resource_reader = nullptr);
    static auto constexpr EndOfText = "<|endoftext|>";
    static auto constexpr FimPrefix = "<|fim_prefix|>";
    static auto constexpr FimMiddle = "<|fim_middle|>";
    static auto constexpr FimSuffix = "<|fim_suffix|>";
    static auto constexpr EndOfPrompt = "<|endofprompt|>";

private:
    static ModelParams r50k_base(const char* resource_name = nullptr, IResourceReader* resource_reader = nullptr);
    static ModelParams p50k_base(const char *resource_name = nullptr, IResourceReader *resource_reader = nullptr);
    static ModelParams p50k_edit(const char *resource_name = nullptr, IResourceReader *resource_reader = nullptr);
    static ModelParams cl100k_base(const char *resource_name = nullptr, IResourceReader *resource_reader = nullptr);
    static ModelParams o200k_base(const char *resource_name = nullptr, IResourceReader *resource_reader = nullptr);
};
