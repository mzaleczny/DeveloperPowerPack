#pragma once

#include "Tilc/configure.h"
#include <Tilc/Utils/ExtString.h>


#if !defined(BUILD_WITHOUT_GRAPHICS)
#define BUILD_WITHOUT_GRAPHICS 0
#endif

#if BUILD_WITHOUT_GRAPHICS == 0
#include <SDL3/SDL.h>
#endif

#include <vector>
#include <sys/types.h>
#include <cstring>
#include <cmath>
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
    constexpr int HiWord(int Value) {
        return (0xffff0000 & Value) >> 16;
    }
    constexpr int LoWord(int Value) {
        return (0x0000ffff & Value);
    }

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

#if BUILD_WITHOUT_GRAPHICS == 0
    inline SDL_Rect FRectToRectFloor(SDL_FRect* Rect)
    {
        SDL_Rect r;
        r.x = static_cast<int>(std::floor(Rect->x));
        r.y = static_cast<int>(std::floor(Rect->y));
        r.w = static_cast<int>(std::floor(Rect->w));
        r.h = static_cast<int>(std::floor(Rect->h));
        return r;
    }

    inline SDL_Rect FRectToRectRound(SDL_FRect* Rect)
    {
        SDL_Rect r;
        r.x = static_cast<int>(std::round(Rect->x));
        r.y = static_cast<int>(std::round(Rect->y));
        r.w = static_cast<int>(std::round(Rect->w));
        r.h = static_cast<int>(std::round(Rect->h));
        return r;
    }
#endif

}
