#include "Tilc/Data/DataProcessor.h"
#include "Tilc/Utils/ExtString.h"
#include "Tilc/Utils/VectorContainer.h"

std::ostream& Tilc::Data::TDataProcessor::PrintList(std::ostream& os, Tilc::Data::TDB& DB, const char* Sql, std::initializer_list<const char*> Fields)
{
    Tilc::TExtString FieldsList{Tilc::Implode(',', Fields)};
    Tilc::TExtString SqlStr{Sql};
    SqlStr.StrReplace("%s", FieldsList);

    Tilc::Data::TDBDataRows Data;
    DB.Select(SqlStr.c_str(), Data);
    os << "{\"items\":";
    Tilc::PrintVectorAsJsonArray(os, Fields, Data);
    os << ",\"items_count\":\"" << Data.size() << "\"";
    //os << ",\"Sql\":\"" << SqlStr << "\"";
    os << "}";
    return os;
}
