#pragma once

#include "Tilc/DllGlobals.h"
#include "Tilc/Utils/ExtString.h"

namespace Tilc
{

    class DECLSPEC TCommandLine : public std::vector<TExtString>
    {
    public:
        TCommandLine();
        TCommandLine(int argc, const char** argv);
        TCommandLine(const char* argv);
        virtual ~TCommandLine();

        int GetParamPos(const char* Param);
        bool HasParam(const char* Param);
        TExtString GetParam(size_t Pos);
        TExtString GetParamValue(const char* Param);
        void RemoveParam(const char* Param);
        size_t GetParamsCount();
        size_t GetItemsCount();

    protected:
        void CommonInit(const char* CommandLine);
    };

}
