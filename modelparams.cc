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
#include "modelparams.h"
#include "embedded_resource_reader.h"
#include <stdexcept>

// ModelParams constructor and member functions

ModelParams::ModelParams() :
    explicit_n_vocab(0)
{
}

ModelParams::ModelParams(int explicit_n_vocab, std::string &&pat_str,
    bpe_encoding_t&& mergeable_ranks,
    std::unordered_map<std::string, int>&& special_tokens) :
    explicit_n_vocab(explicit_n_vocab),
    pat_str(std::move(pat_str)), mergeable_ranks(std::move(mergeable_ranks)), special_tokens(std::move(special_tokens))
{
}

// ModelParamsGenerator member functions

ModelParams ModelParamsGenerator::get_model_params(LanguageModel model, const char *resource_name, IResourceReader *resource_reader)
{
    switch (model) {
        case LanguageModel::O200K_BASE:
            return o200k_base(resource_name, resource_reader);
        case LanguageModel::CL100K_BASE:
            return cl100k_base(resource_name, resource_reader);
        case LanguageModel::P50K_BASE:
            return p50k_base(resource_name, resource_reader);
        case LanguageModel::P50K_EDIT:
            return p50k_edit(resource_name, resource_reader);
        case LanguageModel::R50K_BASE:
            return r50k_base(resource_name, resource_reader);
    }
#if TIKTOKEN_EXCEPTIONS_ENABLE
    throw std::runtime_error("Invalid argument to get_model_params");
#else
    return ModelParams();
#endif
}

#if 0
// latin alphabet only
static auto constexpr o200k_pattern = "[^\\r\\na-zA-Z0-9]?[\\p{Lu}\\p{Lt}\\p{Lm}\\p{Lo}\\p{M}]*[\\p{Ll}\\p{Lm}\\p{Lo}\\p{M}]+(?i:'s|'t|'re|'ve|'m|'ll|'d)?|[^\\r\\na-zA-Z0-9]?[\\p{Lu}\\p{Lt}\\p{Lm}\\p{Lo}\\p{M}]+[\\p{Ll}\\p{Lm}\\p{Lo}\\p{M}]*(?i:'s|'t|'re|'ve|'m|'ll|'d)?|[0-9]{1,3}| ?[^\\sa-zA-Z0-9]+[\\r\\n/]*|\\s*[\\r\\n]+|\\s+(?!\\S)|\\s+";
static auto constexpr cl100k_pattern = "(?:'s|'t|'re|'ve|'m|'ll|'d)|[^\\r\\na-zA-Z0-9]?[a-zA-Z]+|[0-9]{1,3}| ?[^\\sa-zA-Z0-9]+[\\r\\n]*|\\s*[\\r\\n]+|\\s+(?!\\S)|\\s+";
static auto constexpr p50k_pattern = "'s|'t|'re|'ve|'m|'ll|'d| ?[a-zA-Z]+| ?[0-9]+| ?[^\\sa-zA-Z0-9]+|\\s+(?!\\S)|\\s+";
#else
// original regexes don't work with std::regex.  Need to come up with another solution to support other languages.
static auto constexpr o200k_pattern = "[^\\r\\n\\p{L}\\p{N}]?[\\p{Lu}\\p{Lt}\\p{Lm}\\p{Lo}\\p{M}]*[\\p{Ll}\\p{Lm}\\p{Lo}\\p{M}]+(?i:'s|'t|'re|'ve|'m|'ll|'d)?|[^\\r\\n\\p{L}\\p{N}]?[\\p{Lu}\\p{Lt}\\p{Lm}\\p{Lo}\\p{M}]+[\\p{Ll}\\p{Lm}\\p{Lo}\\p{M}]*(?i:'s|'t|'re|'ve|'m|'ll|'d)?|\\p{N}{1,3}| ?[^\\s\\p{L}\\p{N}]+[\\r\\n/]*|\\s*[\\r\\n]+|\\s+(?!\\S)|\\s+";
static auto constexpr cl100k_pattern = "(?i:'s|'t|'re|'ve|'m|'ll|'d)|[^\\r\\n\\p{L}\\p{N}]?\\p{L}+|\\p{N}{1,3}| ?[^\\s\\p{L}\\p{N}]+[\\r\\n]*|\\s*[\\r\\n]+|\\s+(?!\\S)|\\s+";
static auto constexpr p50k_pattern = "'s|'t|'re|'ve|'m|'ll|'d| ?\\p{L}+| ?\\p{N}+| ?[^\\s\\p{L}\\p{N}]+|\\s+(?!\\S)|\\s+";
#endif

constexpr const char* embedded_resource_from_model(LanguageModel model)
{
    constexpr const char* resource_name[(int)LanguageModel::COUNT] = {
        "o200k_base.tiktoken",
        "cl100k_base.tiktoken",
        "r50k_base.tiktoken",
        "p50k_base.tiktoken",
        "p50k_base.tiktoken"
    };

    return resource_name[(int) model];
}

ModelParams ModelParamsGenerator::r50k_base(const char *resource_name, IResourceReader *resource_reader)
{
    resource_name = resource_name ? resource_name : embedded_resource_from_model(LanguageModel::R50K_BASE);

    EmbeddedResourceLoader loader(resource_name, resource_reader);
    return ModelParams(50257, 
        p50k_pattern, 
        loader.loadTokenBytePairEncoding(), 
        { { EndOfText, 50256 } });
}

ModelParams ModelParamsGenerator::p50k_base(const char *resource_name, IResourceReader *resource_reader)
{
    resource_name = resource_name ? resource_name : embedded_resource_from_model(LanguageModel::P50K_BASE);

    EmbeddedResourceLoader loader(resource_name, resource_reader);
    return ModelParams(50281, 
        p50k_pattern, 
        loader.loadTokenBytePairEncoding(), 
        { { EndOfText, 50256 } });
}

ModelParams ModelParamsGenerator::p50k_edit(const char *resource_name, IResourceReader *resource_reader)
{
    resource_name = resource_name ? resource_name : embedded_resource_from_model(LanguageModel::P50K_EDIT);

    EmbeddedResourceLoader loader(resource_name, resource_reader);

    std::unordered_map<std::string, int> specialTokens = { { EndOfText, 50256 }, { FimPrefix, 50281 }, { FimMiddle, 50282 },
        { FimSuffix, 50283 } };

    return ModelParams(0, 
        p50k_pattern, 
        loader.loadTokenBytePairEncoding(), 
        std::move(specialTokens));
}

ModelParams ModelParamsGenerator::cl100k_base(const char *resource_name, IResourceReader *resource_reader)
{
    resource_name = resource_name ? resource_name : embedded_resource_from_model(LanguageModel::CL100K_BASE);

    EmbeddedResourceLoader loader(resource_name, resource_reader);

    std::unordered_map<std::string, int> specialTokens = { { EndOfText, 100257 }, { FimPrefix, 100258 }, { FimMiddle, 100259 },
        { FimSuffix, 100260 }, { EndOfPrompt, 100276 } };

    return ModelParams(0, 
        cl100k_pattern, 
        loader.loadTokenBytePairEncoding(), 
        std::move(specialTokens));
}

ModelParams ModelParamsGenerator::o200k_base(const char *resource_name, IResourceReader *resource_reader)
{
    resource_name = resource_name ? resource_name : embedded_resource_from_model(LanguageModel::O200K_BASE);

    EmbeddedResourceLoader loader(resource_name, resource_reader);

    std::unordered_map<std::string, int> specialTokens = { { EndOfText, 199999 }, { EndOfPrompt, 200018 } };

    return ModelParams(0, 
        o200k_pattern, 
        loader.loadTokenBytePairEncoding(), 
        std::move(specialTokens));
}
