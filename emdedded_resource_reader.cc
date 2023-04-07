//
// Created by markt on 4/5/23.
//

#include "emdedded_resource_reader.h"
#include "encoding_utils.h"

#include <fstream>
#include <sstream>
#include <stdexcept>

std::vector<std::string> EmbeddedResourceReader::readEmbeddedResourceAsLines(const std::string& resourceName) {
    std::ifstream file(resourceName);
    if (!file.is_open()) {
        throw std::runtime_error("Embedded resource '" + resourceName + "' not found.");
    }

    std::string line;
    std::vector<std::string> lines;
    while (std::getline(file, line)) {
        lines.push_back(line);
    }

    return lines;
}

std::unordered_map<std::vector<uint8_t>, int, VectorHash> EmbeddedResourceReader::loadTokenBytePairEncoding(const std::string& dataSourceName) {
    auto lines = readEmbeddedResourceAsLines(dataSourceName);
    std::unordered_map<std::vector<uint8_t>, int, VectorHash> token_byte_pair_encoding;

    for (const auto& line : lines) {
        if (!line.empty()) {
            std::istringstream iss(line);
            std::string base64_string;
            int rank;

            iss >> base64_string >> rank;
            auto decoded = base64::decode(base64_string);
            std::vector<uint8_t> decoded_bytes(decoded.begin(), decoded.end());

            token_byte_pair_encoding[decoded_bytes] = rank;
        }
    }

    return token_byte_pair_encoding;
}
