
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <regex>
#include "byte_pair_encoding.h"
#include "modelparams.h"
#include "modelparams.h"

class GptEncoding {
public:
    GptEncoding(const std::string& pattern_string,
                const std::unordered_map<std::vector<uint8_t>, int, VectorHash>& byte_pair_ranks,
                const std::unordered_map<std::string, int>& special_token_mappings,
                int explicit_n_vocab);

    static std::shared_ptr<GptEncoding> get_encoding(LanguageModel model);

    std::vector<int> encode(const std::string& line_to_encode,
                            const std::unordered_set<std::string>& allowed_special = {},
                            const std::unordered_set<std::string>& disallowed_special = {"all"});

    std::string decode(const std::vector<int>& input_tokens_to_decode);

private:
    int max_token_value_;
    std::unordered_map<std::string, int> special_token_mappings_;
    BytePairEncodingCore byte_pair_encoding_core_processor_;
};
