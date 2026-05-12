#include "Tilc/OS/SystemUtils.h"
#include "Tilc/Utils/ExtString.h"
#include <sstream>
#include <iostream>
#include "SDL3/SDL.h"

#ifdef _WINDOWS
#include <Windows.h>
#endif


void* Tilc::OS::GetActiveWindowSystemHandle()
{
#ifdef _WINDOWS
    return ::GetActiveWindow();
#elif ANDROID
    return nullptr;
#else
    return nullptr;
#endif
}
