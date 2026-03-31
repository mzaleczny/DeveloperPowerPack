#include "Tilc/Utils/WindowsStuff.h"
#include <iostream>

#ifndef _WINDOWS
int GetActiveWindow() { return 0; }
int GetLastError() { return 0; }

int MessageBoxA(int ActiveWindow, const char* Message, const char* Title, int Options)
{
    std::cout << "*** " << Title << " ***" << std::endl;
    std::cout << Message << "\n" << std::endl;
    return 0;
}
#endif
