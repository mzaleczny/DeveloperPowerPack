#include "Tilc/Data/DataProcessor.h"
#include "Tilc/Utils/ExtString.h"
#include "Tilc/Utils/VectorContainer.h"
#include "Tilc/Utils/JsonParser.h"
#include "Tilc/Utils/StdObject.h"

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

std::ostream& Tilc::Data::TDataProcessor::DeleteItems(std::ostream& os, Tilc::Data::TDB& DB, const char* Sql, const Tilc::TExtString& JsonCommand)
{
    Tilc::TJsonParser Parser;
    Tilc::TStdObject* JsonInput = Parser.parse(JsonCommand);
    if (JsonInput)
    {
        Tilc::TExtString Ids;
        Tilc::TExtString CommandAction = JsonInput->getAsObject("root")->getAsString("Action");
        if (CommandAction == "Delete")
        {
            Tilc::TPropertiesVector* IdsVec = JsonInput->getAsObject("root")->getAsArray("Ids");
            for (size_t i = 0; i < IdsVec->size(); ++i)
            {
                if ((*IdsVec)[i]->type == Tilc::PROPERTY_VALUE_INT)
                {
                    if (i > 0)
                    {
                        Ids += ",";
                    }
                    Ids += std::to_string((*IdsVec)[i]->iValue);
                }
            }
            if (Ids.length() > 0)
            {
                DB.ExecQuery(Tilc::TExtString("DELETE FROM categories WHERE id IN (" + Ids + ")").c_str());
            }
        }
        delete JsonInput;
    }
    return os;
}
