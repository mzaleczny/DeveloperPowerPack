#include "Tilc/Globals.h"
#include "Tilc/OS/SystemUtils.h"
#include "Tilc/Utils/ExtString.h"
#include <sstream>
#include <iostream>

#if BUILD_WITHOUT_GRAPHICS == 0
#include <SDL3/SDL.h>
#endif

#ifdef _WINDOWS
#include <Windows.h>
#include <errno.h>
#endif


DECLSPEC void* Tilc::OS::GetActiveWindowSystemHandle()
{
#ifdef _WINDOWS
    return ::GetActiveWindow();
#elif ANDROID
    return nullptr;
#else
    return nullptr;
#endif
}



DECLSPEC int Tilc::OS::nanosleep(const timespec* req, timespec* rem)
{
#ifdef _WINDOWS
    if (!req || req->tv_nsec < 0 || req->tv_nsec >= 1000000000L)
    {
        return EINVAL;
    }

    LONGLONG ticks = -(
        static_cast<LONGLONG>(req->tv_sec) * 10000000LL +
        static_cast<LONGLONG>(req->tv_nsec) / 100LL
    );
    HANDLE timer = CreateWaitableTimerA(NULL, TRUE, NULL);
    if (!timer)
    {
        return -1;
    }

    LARGE_INTEGER li;
    li.QuadPart = ticks;

    if (!SetWaitableTimer(timer, &li, 0, NULL, NULL, FALSE))
    {
        CloseHandle(timer);
        return -1;
    }

    DWORD result = WaitForSingleObject(timer, INFINITE);
    CloseHandle(timer);

    if (result == WAIT_OBJECT_0)
    {
        return 0;
    }

    if (rem)
    {
        rem->tv_sec = 0;
        rem->tv_nsec = 0;
    }

    errno = EINTR;
    return -1;
#elif ANDROID
    return 0;
#elifdef __linux__
    return ::nanosleep(req, rem);
#endif
    return 0;
}
