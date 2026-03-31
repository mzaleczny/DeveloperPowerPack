#pragma once

#include "Tilc/DllGlobals.h"
#include "Tilc/Utils/ExtString.h"

namespace Tilc
{
    using TWideChar = int;
    using TMBTOWC = int (*)(TWideChar* OutputWideChar, const unsigned char* Input, size_t InputSize);
    using TWCTOMB = int (*)(unsigned char* Result, size_t ResultSize, TWideChar wc);
    enum class EErrorCode
    {
        EEC_ERROR_ILSEQ = -1,
        EEC_ERROR_TOOFEW = -2,
        EEC_ERROR_INVALID_UNICODE = -3,
        EEC_ERROR_TOOSMALL = -4,
        EEC_NOERROR = 0
    };
    inline int EErrorCodeToInt(EErrorCode Val)
    {
        return static_cast<int>(Val);
    }

    class DECLSPEC TCPConv
    {
    public:
        static TExtString Convert(const TExtString& Input, const char* FromCodePage, const char* ToCodePage);
        // Converts next char from Input and stores result WideChar in OutputWideChar. Returns:
        // positive value: number of bytes of outputchar that is stored in OutputWideChar
        // negative value: error code
        static int Utf8_mbtowc(TWideChar* OutputWideChar, const unsigned char* Input, size_t InputSize);
        // Converts TWideChar wc to multibyte sequence stored in Result. Size of buffer Result (which must allow inlcuding null terminated byte)
        // is specified in ResultSize.
        // Returns number of bytes stored in Result or negative value if error occured.
        static int Utf8_wctomb(unsigned char* Result, size_t ResultSize, TWideChar wc);


        // Converts next char from Input and stores result WideChar in OutputWideChar. Returns:
        // positive value: number of bytes of outputchar that is stored in OutputWideChar
        // negative value: error code
        static const unsigned short cp852_2uni[128];
        static int CP852_mbtowc(TWideChar* OutputWideChar, const unsigned char* Input, size_t InputSize);
        // Converts TWideChar wc to multibyte sequence stored in Result. Size of buffer Result (which must allow inlcuding null terminated byte)
        // is specified in ResultSize.
        // Returns number of bytes stored in Result or negative value if error occured.
        static const unsigned char cp852_page00[224];
        static const unsigned char cp852_page02[32];
        static const unsigned char cp852_page25[168];
        static int CP852_wctomb(unsigned char* Result, size_t ResultSize, TWideChar wc);


        // Converts next char from Input and stores result WideChar in OutputWideChar. Returns:
        // positive value: number of bytes of outputchar that is stored in OutputWideChar
        // negative value: error code
        static const unsigned short cp1250_2uni[128];
        static int CP1250_mbtowc(TWideChar* OutputWideChar, const unsigned char* Input, size_t InputSize);
        // Converts TWideChar wc to multibyte sequence stored in Result. Size of buffer Result (which must allow inlcuding null terminated byte)
        // is specified in ResultSize.
        // Returns number of bytes stored in Result or negative value if error occured.
        static const unsigned char cp1250_page00[224];
        static const unsigned char cp1250_page02[32];
        static const unsigned char cp1250_page20[48];
        static int CP1250_wctomb(unsigned char* Result, size_t ResultSize, TWideChar wc);
    };
}
