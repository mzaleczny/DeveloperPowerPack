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
    Tilc::TExtString Error = DB.GetErrorMessage();
    if (!Error.empty())
    {
        os << Error << ": " << SqlStr << "\n";
    }

    os << "{\"items\":";
    Tilc::PrintVectorAsJsonArray(os, Fields, Data);
    os << ",\"items_count\":\"" << Data.size() << "\"";
    //os << ",\"Sql\":\"" << SqlStr << "\"";
    os << "}";
    return os;
}

std::ostream& Tilc::Data::TDataProcessor::SaveItems(std::ostream& os, Tilc::Data::TDB& DB, const char* TableName, const Tilc::TExtString& JsonCommand)
{
    Tilc::TJsonParser Parser;
    Tilc::TStdObject* JsonInput = Parser.parse(JsonCommand);
    if (JsonInput)
    {
        Tilc::TExtString CommandAction = JsonInput->getAsObject("root")->getAsString("Action");
        if (CommandAction == "Save")
        {
            Tilc::TPropertiesVector* ItemsVec = JsonInput->getAsObject("root")->getAsArray("Items");
            for (size_t i = 0; i < ItemsVec->size(); ++i)
            {
                if ((*ItemsVec)[i] && (*ItemsVec)[i]->oValue)
                {
                    TStdObject* Item = (*ItemsVec)[i]->oValue;
                    TPropertiesVector& Fields = Item->getProperties();
                    Tilc::TExtString Id, IdFieldName, Sql;
                    std::vector<Tilc::TExtString> FieldNames;
                    std::vector<Tilc::TExtString> FieldValues;
                    Tilc::Data::TDBFieldTypes FieldTypes;
                    for (size_t j = 0; j < Fields.size(); ++j)
                    {
                        TStdObjectProperty* Field = Fields[j];
                        if (Field->name == "id" || Field->name == "Id" || Field->name == "ID")
                        {
                            Id = Field->getAsString();
                            IdFieldName = Field->name;
                            if (Id.empty() || Field->iValue < 1)
                            {
                                Id = "";
                                IdFieldName = "";
                            }
                        }
                        else
                        {
                            FieldNames.push_back(Field->name);
                            FieldValues.push_back(Field->getAsString());
                            FieldTypes.push_back(Tilc::Data::EDBFT_STRING);
                        }
                    }

                    if (Id.empty())
                    {
                        Tilc::TExtString FieldNamesString = Tilc::Implode(',', FieldNames);
                        Tilc::TExtString Placeholders = Tilc::ImplodeRepeatedString(',', "?", FieldNames.size());
                        Sql = Tilc::TExtString("INSERT INTO ") + TableName + " (" + FieldNamesString + ") VALUES (" + Placeholders + ")";
                    }
                    else
                    {
                        Tilc::TExtString FieldPairs;
                        Sql = Tilc::TExtString("UPDATE ") + TableName + " SET ";
                        for (size_t k = 0; k < FieldNames.size(); ++k)
                        {
                            Sql += FieldNames[k] + "=?";
                            if (k < FieldNames.size() - 1)
                            {
                                Sql += ",";
                            }
                        }
                        Sql += " WHERE " + IdFieldName + "=" + Id;
                    }

                    if (Sql.length() > 0)
                    {
                        DB.ExecQuery(Sql.c_str(), FieldTypes, FieldValues);
                    }
                }
            }
        }
        delete JsonInput;
    }
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
                Tilc::TExtString SqlStr{Sql};
                SqlStr.StrReplace("%s", Ids);
                DB.ExecQuery(SqlStr.c_str());
            }
        }
        delete JsonInput;
    }
    return os;
}
