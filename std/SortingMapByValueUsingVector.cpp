#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>

int main(int argc, char *argv[])
{
    std::map<std::string, float> Averages;
    Averages.emplace("Piotr", 12.0f);
    Averages.emplace("Marcin", 1.0f);
    Averages.emplace("Tomasz", 5.0f);

    for (auto& it : Averages)
    {
        std::cout << it.first << ": " << it.second << std::endl;
    }
    std::cout << "---------" << std::endl;

    std::vector<std::pair<std::string, float>> v;
    v.reserve(Averages.size());
    for (auto& it : Averages)
    {
        v.push_back(it);
    }
    std::sort(v.begin(), v.end(), [](const std::pair<std::string, float>& v1, const std::pair<std::string, float>& v2) { return v1.second < v2.second; });
    for (auto& it : v)
    {
        std::cout << it.first << ": " << it.second << std::endl;
    }

    return 0;
}