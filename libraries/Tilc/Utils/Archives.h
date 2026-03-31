#pragma once

#include "Tilc/DllGlobals.h"
#include "Tilc/Utils/ExtString.h"

namespace Tilc
{
    class DECLSPEC TArchive
    {
    public:
        static int GzUncompress(const char* GzFilePath, TExtString& Output);
        static int GzUncompress(const TExtString& Input, TExtString& Output);
        static int GzCompress(const TExtString& Input, TExtString& Output);
    };
}
