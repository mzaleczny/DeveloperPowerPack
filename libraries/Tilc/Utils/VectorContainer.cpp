#include "Tilc/Utils/VectorContainer.h"

DECLSPEC std::ostream& Tilc::PrintVectorAsJsonArray(std::ostream& os, std::initializer_list<const char*> FieldNames, const Tilc::Data::TDBDataRows& Items)
{
    os << "[";
    for (size_t i = 0; i < Items.size(); ++i)
    {
        if (i > 0)
        {
            os << ",";
        }
        os  << "{";
        size_t j = 0;
        for (auto it = FieldNames.begin(); it != FieldNames.end(); ++it)
        {
            os << "\"" << *it << "\":\"" << Items[i][j] << "\"";
            if (j < Items[i].size() - 1)
            {
                os << ",";
            }
            ++j;
        }
        os << "}";
    }
    os << "]";
    return os;
}