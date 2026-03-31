#include <string>
#include <iostream>
#include <vector>
#include <filesystem>
#include "Tilc/Tilc.h"

int main(int argc, const char** argv)
{
    Tilc::TJsonParser JsonParser;
    Tilc::TFile File("assets/crossword.json");
    Tilc::TExtString JsonContent;
    File.ReadContent(JsonContent);
    int Count = 100000;
    for (int i = 0; i < Count; ++i)
    {
        Tilc::TStdObject* Json = JsonParser.parse(JsonContent);
        if (Json)
        {
            if (Count == 1)
            {
                std::cout << Json->toJson() << std::endl;
            }
            delete Json;
        }
    }
    return 0;
}
