#include "encoding_utils.h"

static std::string base64_chars =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789+/";

namespace base64 {
static auto constexpr fillchar = '=';

static std::string cvt = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                         "abcdefghijklmnopqrstuvwxyz"
                         "0123456789+/";

std::vector<uint8_t> decode(const std::string &data)
{
    std::string::size_type i;
    char c;
    char c1;
    std::string::size_type len = data.length();
    std::vector<uint8_t> ret;

    for (i = 0; i < len; ++i) {
        c = (char) cvt.find(data[i]);
        ++i;
        c1 = (char) cvt.find(data[i]);
        c = (c << 2) | ((c1 >> 4) & 0x3);
        ret.push_back(c);
        if (++i < len) {
            c = data[i];
            if (fillchar == c)
                break;
            c = (char) cvt.find(c);
            c1 = ((c1 << 4) & 0xf0) | ((c >> 2) & 0xf);
            ret.push_back(c1);
        }
        if (++i < len) {
            c1 = data[i];
            if (fillchar == c1)
                break;
            c1 = (char) cvt.find(c1);
            c = ((c << 6) & 0xc0) | c1;
            ret.push_back(c);
        }
    }
    return (ret);
}


} // base64

