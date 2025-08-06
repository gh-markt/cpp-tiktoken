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
#include "embedded_resource_reader.h"
#include "encoding_utils.h"

#include <filesystem>
#include <fstream>
#include <sstream>
#include <stdexcept>

#ifndef TIKTOKEN_EMBEDDED_RESOURCES
#ifdef _WIN32
#include <windows.h>
#else
#include <limits.h>
#include <unistd.h>
#endif
#endif

#if defined(TIKTOKEN_EMBEDDED_RESOURCES)
extern std::pair<const unsigned char *, size_t> get_resource_cl100k_base();
extern std::pair<const unsigned char *, size_t> get_resource_o200k_base();
extern std::pair<const unsigned char *, size_t> get_resource_p50k_base();
extern std::pair<const unsigned char *, size_t> get_resource_r50k_base();
#endif

namespace 
{
#ifndef TIKTOKEN_EMBEDDED_RESOURCES
    std::filesystem::path get_exe_parent_path_intern() {
        std::filesystem::path path;
#ifdef _WIN32
        wchar_t result[MAX_PATH] = {0};
        GetModuleFileNameW(nullptr, result, MAX_PATH);
        path = std::filesystem::path(result);
#else
        char result[PATH_MAX];
        ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
        path = std::filesystem::path(std::string(result, count > 0 ? count : 0));
#endif
        return path.parent_path();
    }

    static const std::filesystem::path g_exe_parent_path = get_exe_parent_path_intern();
#endif

    class EmbeddedResourceReader: public IResourceReader {
    public:
        std::vector<std::string> readLines(std::string_view resourceName) override;
    };

    std::vector<std::string> EmbeddedResourceReader::readLines(std::string_view resourceName)
    {
#ifndef TIKTOKEN_EMBEDDED_RESOURCES
        std::filesystem::path resource_path = g_exe_parent_path / "tokenizers" / resourceName;
        std::ifstream file(resource_path);
        if (!file.is_open()) {
#if TIKTOKEN_EXCEPTIONS_ENABLE
            throw std::runtime_error("Embedded resource '" + resource_path.string() + "' not found.");
#else
            return {};
#endif
        }

        std::string line;
        std::vector<std::string> lines;
        while (std::getline(file, line)) {
            lines.push_back(line);
        }

        return lines;
#else
        auto readLinesFromMem = [](std::pair<const unsigned char *, size_t> mem) {
            struct membuf: std::streambuf {
                membuf(char *base, std::ptrdiff_t n)
                {
                    this->setg(base, base, base + n);
                }
            };

            membuf sbuf(const_cast<char *>((const char*) mem.first), (ptrdiff_t) mem.second);
            std::istream file(&sbuf);

            std::string line;
            std::vector<std::string> lines;
            while (std::getline(file, line))
                lines.push_back(line);

            return lines;
        };

        if (resourceName == "o200k_base.tiktoken")
        {
            return readLinesFromMem(get_resource_o200k_base());
        }
        else if (resourceName == "cl100k_base.tiktoken") 
        {
            return readLinesFromMem(get_resource_cl100k_base());
        }
        else if (resourceName == "r50k_base.tiktoken") 
        {
            return readLinesFromMem(get_resource_r50k_base());
        }
        else if (resourceName == "p50k_base.tiktoken") 
        {
            return readLinesFromMem(get_resource_p50k_base());
        } 
        else
        {
#if TIKTOKEN_EXCEPTIONS_ENABLE
            throw std::runtime_error("Embedded resource '" + (std::string) resourceName + "' not found.");
#else
            return {};
#endif
        }
        
#endif
    }
}

EmbeddedResourceLoader::EmbeddedResourceLoader(const std::string& dataSourceName, IResourceReader* reader)
    : resourceReader_(reader)
    , dataSourceName_(dataSourceName)
{
}

std::vector<std::string> EmbeddedResourceLoader::readEmbeddedResourceAsLines() {
    if (!!resourceReader_) {
        return resourceReader_->readLines(dataSourceName_);
    }
    return EmbeddedResourceReader().readLines(dataSourceName_);
}

bpe_encoding_t
EmbeddedResourceLoader::loadTokenBytePairEncoding()
{
    auto lines = readEmbeddedResourceAsLines();
    bpe_encoding_t token_byte_pair_encoding;

    for (const auto &line: lines) {
        if (!line.empty()) {
            std::istringstream iss(line);

            std::string base64_string;
            int rank;

            iss >> base64_string >> rank;

            auto decoded = base64::decode(base64_string);
            token_byte_pair_encoding.insert({std::move(decoded), rank});
        }
    }

    return token_byte_pair_encoding;
}
