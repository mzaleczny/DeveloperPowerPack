#include <string>
#include <iostream>
#include <vector>
#include <filesystem>
#include "Tilc/Tilc.h"

int main(int argc, const char** argv)
{
    Tilc::TCommandLine CommandLine(argc, argv);
    //const char* args[6]{ "cat", "PL_WINDOWS1250.txt", "-f", "cp1250", "-t", "cp852" };
    //Tilc::TCommandLine CommandLine(6, args);
    Tilc::TExtString FileName;
    Tilc::TExtString FromEncoding("utf8");
    Tilc::TExtString ToEncoding("cp852");

    if (CommandLine.HasParam("-f"))
    {
        FromEncoding = CommandLine.GetParamValue("-f");
        CommandLine.RemoveParam("-f");
    }
    if (CommandLine.HasParam("-t"))
    {
        ToEncoding = CommandLine.GetParamValue("-t");
        CommandLine.RemoveParam("-t");
    }

    while (CommandLine.size() > 0)
    {
        FileName = CommandLine[0];
        CommandLine.erase(CommandLine.begin());

        Tilc::TFile File(FileName.c_str());
        Tilc::TExtString Content;
        File.ReadContent(Content);

        if (FromEncoding == ToEncoding)
        {
            std::cout << Content;
        }
        else
        {
            std::cout << Tilc::TCPConv::Convert(Content, FromEncoding.c_str(), ToEncoding.c_str());
        }
    }

    return 0;
}
