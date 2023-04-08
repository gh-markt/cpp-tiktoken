#pragma once
#include <string>
#include <vector>
#include <cstring>
#define PCRE2_CODE_UNIT_WIDTH 0

#include <pcre2.h>

class PCRERegex {
public:
    PCRERegex(const std::string& pattern, int flags);
    PCRERegex(const PCRERegex &) = delete;
    ~PCRERegex();

    std::vector<std::string> all_matches(const std::string& text) const;


private:
    pcre2_code_8* regex_ = nullptr;
};
