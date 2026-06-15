#pragma once
#include "Tilc/DllGlobals.h"

#ifdef _WINDOWS
#include <time.h>
#endif

namespace Tilc::OS
{
#ifdef _WINDOWS
    struct timespec
    {
        time_t tv_sec; // sekundy
        long tv_nsec;  // nanosekundy
    };
#endif

    DECLSPEC void* GetActiveWindowSystemHandle();
    DECLSPEC int nanosleep(const struct timespec* req, struct timespec* rem);
}
