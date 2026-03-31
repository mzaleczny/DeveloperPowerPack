#include "Tilc/Utils/Crypto.h"
#include "Tilc/Utils/ExtString.h"

#define SIZE        4096



char* Tilc::StringXor(char* str, size_t len, const char c) {
    for (size_t i = 0; i < len; i++)
    {
        str[i] ^= c;
    }

    return str;
}



char* Tilc::StringXorStr(char* str, size_t len, const char* chars)
{
    size_t chars_len = 0;
    size_t num_char = 0;

    chars_len = strlen(chars);

    for (size_t i = 0; i < len; i++) {
        str[i] ^= chars[num_char];
        num_char = (num_char + 1) % chars_len;
    }

    return str;
}
