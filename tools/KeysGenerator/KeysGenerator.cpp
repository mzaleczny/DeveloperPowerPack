#include <iostream>
#include <string>
using namespace std;

const constexpr int CHARTYPE_LETTER = 0;
const constexpr int CHARTYPE_DIGIT = 1;

std::string GenerateKey(int NumGroups, int GroupLength);

int main(int argc, char *argv[])
{
    srand(time(nullptr));
    for (int i = 0; i < 100; ++i)
    {
        cout << GenerateKey(5, 4) << endl;
    }
    return 0;
}

string GenerateKey(int NumGroups, int GroupLength)
{
    string Result;
    for (int i = 0; i < NumGroups; ++i)
    {
        if (i > 0)
        {
            Result += '-';
        }

        for (int j = 0; j < GroupLength; ++j)
        {
            int CharType;
            int RandomValue = rand() % 100;
            if (RandomValue < 40)
            {
                CharType = CHARTYPE_LETTER;
            }
            else
            {
                CharType = CHARTYPE_DIGIT;
            }

            switch (CharType)
            {
            case CHARTYPE_LETTER:
                Result += 'A' + rand() % 26;
                break;
            case CHARTYPE_DIGIT:
                Result += '0' + rand() % 10;
                break;
            }
        }
    }
    return Result;
}
