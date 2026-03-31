#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <filesystem>
#include "Tilc/Tilc.h"

int main(int argc, const char** argv)
{
    Tilc::TCommandLine CommandLine(argc, argv);
    //const char* args[6]{ "cat", "PL_WINDOWS1250.txt", "-f", "cp1250", "-t", "cp852" };
    //Tilc::TCommandLine CommandLine(6, args);
    Tilc::TExtString FileName;
    int DoCompress = -1;

    if (CommandLine.HasParam("-c"))
    {
        DoCompress = 1;
        CommandLine.erase(CommandLine.begin() + CommandLine.GetParamPos("-c"));
    }
    if (CommandLine.HasParam("-d"))
    {
        DoCompress = 0;
        CommandLine.erase(CommandLine.begin() + CommandLine.GetParamPos("-d"));
    }

    if (DoCompress == -1)
    {
        std::cerr << "Specify if you want to compress or decompress" << std::endl;
        return -1;
    }

    while (CommandLine.size() > 0)
    {
        FileName = CommandLine[0];
        CommandLine.erase(CommandLine.begin());

        Tilc::TFile File(FileName.c_str(), std::ios::in | std::ios::binary);
        Tilc::TExtString Content, Output;
        File.ReadContent(Content);

        if (DoCompress == 1)
        {
            std::cout << "Compressing file: " << FileName << std::endl;
            Tilc::TArchive::GzCompress(Content, Output);
            Tilc::TFile OutFile((FileName + ".gz").c_str(), std::ios::out | std::ios::binary);
            OutFile.WriteContent(Output);
        }
        else if (DoCompress == 0)
        {
            std::cout << "Decompressing file: " << FileName << std::endl;
            Tilc::TArchive::GzUncompress(Content, Output);
            Tilc::TFile OutFile(FileName.substr(0, FileName.length()-3).c_str(), std::ios::out | std::ios::binary);
            OutFile.WriteContent(Output);
        }
    }

    return 0;
}
