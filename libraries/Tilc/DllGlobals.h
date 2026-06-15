#pragma once

#ifndef DECLSPEC
    #if defined(_WIN32)
        #if defined(DLL_EXPORT)
            #define DECLSPEC __declspec(dllexport)
        #else
            #define DECLSPEC __declspec(dllimport)
        #endif
    #else // non windows
        #define DECLSPEC
    #endif
#endif
