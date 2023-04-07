#pragma once
#include "encoding_utils.h"
#include <string>
#include <unordered_map>
#include <vector>

class EmbeddedResourceReader {
public:
    static std::unordered_map<std::vector<uint8_t>, int, VectorHash> loadTokenBytePairEncoding(const std::string& dataSourceName);

private:
    static std::vector<std::string> readEmbeddedResourceAsLines(const std::string& resourceName);
};
