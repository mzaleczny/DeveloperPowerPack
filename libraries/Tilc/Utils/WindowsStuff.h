#pragma once
#include "Tilc/DllGlobals.h"

#ifndef _WINDOWS
    #define MB_OK 0
    #define MB_YESNO 0
    #define MB_YESNOCANCEL 0
    #define MB_ICONINFORMATION 0
    #define MB_ICONERROR 0
    #define MB_ICONQUESTION 0
    #define MB_ICONWARNING 0
    int DECLSPEC GetActiveWindow();
    int DECLSPEC GetLastError();
    int DECLSPEC MessageBoxA(int ActiveWindow, const char* Message, const char* Title, int Options);
#endif
