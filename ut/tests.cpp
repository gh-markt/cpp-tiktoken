#include "encoding.h"
#include "emdedded_resource_reader.h"

#include "gtest/gtest.h"

#include <fstream>

class TFilePathResourceReader : public IResourceReader {
public:
    TFilePathResourceReader(const std::string& path) 
        : path_(path)
    {
    }

    std::vector<std::string> readLines() override {
        std::ifstream file(path_);
        if (!file.is_open()) {
            throw std::runtime_error("Embedded resource '" + path_ + "' not found.");
        }

        std::string line;
        std::vector<std::string> lines;
        while (std::getline(file, line)) {
            lines.push_back(line);
        }

        return lines;
    }
private:
    std::string path_;
};

TEST(TestGetEncoding, TestDefaultEncod)
{
    auto encoder = GptEncoding::get_encoding(LanguageModel::CL100K_BASE);
}

TEST(TestGetEncoding, TestCustomResourceReader)
{
    TFilePathResourceReader reader("../tokenizers/cl100k_base.tiktoken");
    auto encoder = GptEncoding::get_encoding(LanguageModel::CL100K_BASE, &reader);
}
