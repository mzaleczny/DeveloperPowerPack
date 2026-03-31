#pragma once

#include <Tilc/Utils/ExtString.h>
#include <vector>
#include <sys/types.h>
#include <cstring>
#include <random>

#define STRING2(x)  #x
#define STRING(x)   STRING2(x)

#ifndef COMMON_MEASURE_STRING
#define COMMON_MEASURE_STRING   L"~!@#$%^&*()_+|`1234567890-=\\QWERTYUIOPASDFGHJKLZXCVBNMqwertyuiopasdfghjklzxcvbnm{}[]:\";'<>?,./ĄŚĘŁÓŃŹŻąśęłóńćźż"
#endif

#ifndef COMMON_EMPTY_STRING
#define COMMON_EMPTY_STRING   L""
#endif

namespace Tilc
{

    constexpr const int InitFlag_FullScreen = 1;
    constexpr const int InitFlag_WithStreamingTexture = 2;
    constexpr const int InitFlag_WindowBorderless = 4;

    constexpr const char* Chars = "1234567890qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM!@#$%^&*()-=_+[]{};:\'\"\\|,<.>/?";

    enum class EMessageBoxType
    {
        EMBT_OK,
        EMBT_YesNo,
        EMBT_Custom
    };
    enum class EMessageBoxResult
    {
        EMBR_OK,
        EMBR_Yes,
        EMBR_No
    };

    using TPointersVector = std::vector<void*>;
    using TStringVector = std::vector<Tilc::TExtString>;

    extern std::default_random_engine RandomGenerator;
}
