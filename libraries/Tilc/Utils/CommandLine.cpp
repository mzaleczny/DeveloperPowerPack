#include "Tilc/Utils/CommandLine.h"
#include <algorithm>
#include <string.h>

namespace Tilc
{

#define PARAM_NAME_STARTING_CHARS       "-/"


void TCommandLine::CommonInit(const char* CommandLine)
{
    if (CommandLine)
    {
        size_t i;
        size_t len = strlen(CommandLine);
        
        bool ParamStarted = false;
        char CurChar;
        TExtString Param;
        
        for (i = 0; i < len; i++)
        {
            CurChar = CommandLine[i];
            if (CurChar == '"')
            {
                if (!ParamStarted)
                {
                    ParamStarted = true;
                }
                else
                {
                    if (Param.length() > 0)
                    {
                        push_back(Param);
                    }
                    Param = "";
                    ParamStarted = false;
                }
                continue;
            }
            
            if (std::isspace(CurChar))
            {
                if (ParamStarted)
                {
                    Param += CurChar;
                }
                else
                {
                    if (Param.length() > 0)
                    {
                        push_back(Param);
                    }
                    Param = "";
                }
                continue;
            }
            
            Param.push_back(CurChar);
        }
        
        if (Param.length() > 0)
        {
            push_back(Param);
        }
    }
}

TCommandLine::TCommandLine()
{
    CommonInit(nullptr);
}

TCommandLine::TCommandLine(int argc, const char** argv)
{
    Tilc::TExtString CommandLine;
    for (size_t i = 1; i < argc; i++)
    {
        // we skip program name in first array element
        Tilc::TExtString Param(argv[i], strlen(argv[i]));
        if (Param.ContainsWhiteSpace())
        {
            CommandLine += Tilc::TExtString("\"") + argv[i] + "\"";
        }
        else
        {
            CommandLine += argv[i];
        }
        if (i < argc-1)
        {
            CommandLine += " ";
        }
    }
    
    CommonInit(CommandLine.c_str());
}

TCommandLine::TCommandLine(const char* commandLine)
{
    CommonInit(commandLine);
}

TCommandLine::~TCommandLine()
{
}

int TCommandLine::GetParamPos(const char* Param)
{
    if (!Param || strlen(Param) < 1)
    {
        return -1;
    }
    
    TExtString sparam(Param);
    TExtString cparam;
    sparam.LTrimAllChars(PARAM_NAME_STARTING_CHARS);
    
    size_t size = this->size();
    for (size_t i = 0; i < size; ++i)
    {
        cparam = (*this)[i];
        cparam.LTrimAllChars(PARAM_NAME_STARTING_CHARS);
        if (cparam == sparam)
        {
            return i;
        }
    }
    
    return -1;
}

bool TCommandLine::HasParam(const char* Param)
{
    return GetParamPos(Param) != -1;
}

TExtString TCommandLine::GetParam(size_t Pos)
{
    if (Pos < this->size())
    {
        return (*this)[Pos];
    }
    return "";
}

TExtString TCommandLine::GetParamValue(const char* Param)
{
    long long ParamPos = GetParamPos(Param);
    if (ParamPos == -1 || ParamPos + 1 == size())
    {
        return "";
    }
    
    return (*this)[ParamPos+1];
}

void TCommandLine::RemoveParam(const char* Param)
{
    long long ParamPos = GetParamPos(Param);
    if (ParamPos == -1)
    {
        return;
    }
    
    auto it = std::find(begin(), end(), Param);
    it = erase(it);
    while (it != end() && (*it)[0] != '-')
    {
        it = erase(it);
    }
}

size_t TCommandLine::GetParamsCount()
{
    if (size() < 1)
    {
        return 0;
    }
    
    size_t size = this->size();
    size_t count = 0;
    for (size_t i = 0; i < size; ++i)
    {
        if ((*this)[i].StartsWithOneOfChars(PARAM_NAME_STARTING_CHARS)) {
            count++;
        }
    }
    
    return count;
}

size_t TCommandLine::GetItemsCount()
{
    return size();
}

}
