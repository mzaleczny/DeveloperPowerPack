#include "Tilc/Data/DataProcessor.h"
#include "Tilc/Utils/ExtString.h"
#include "Tilc/Utils/VectorContainer.h"
#include "Tilc/Utils/JsonParser.h"
#include "Tilc/Utils/StdObject.h"
#include "Tilc/Data/DataDbResources.h"
#include "Tilc/Data/DbSQLite.h"

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

std::ostream& Tilc::Data::TDataProcessor::PrintList(std::ostream& os, Tilc::Data::TDB& DB, const char* Sql, const std::vector<const char*>& Fields)
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
                    Tilc::TExtString Id, IdFieldName, Sql, Pictures, NewPictures;
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
                        else if (Field->name != "pictures" && Field->name != "new_pictures")
                        {
                            FieldNames.push_back(Field->name);
                            FieldValues.push_back(Field->getAsString());
                            FieldTypes.push_back(Tilc::Data::EDBFT_STRING);
                        }
                        else
                        {
                            if (Field->name == "pictures")
                            {
                                Pictures = Field->getAsString();
                            }
                            else if (Field->name == "new_pictures")
                            {
                                NewPictures = Field->getAsString();
                            }
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
                        // jeśli Id jest puste, to musimy pobrać id ostatnio dodanego rekordu
                        if (Id.empty())
                        {
                            Id = std::to_string(DB.GetLastInsertId());
                        }
                        // Teraz przenosimy nowe zdjęcia do pliku zdjęć encji, jeśli został podany
                        if (!NewPictures.empty())
                        {
                            Tilc::Data::TDataProcessor::CopyPictures("products", Id, NewPictures, "miniatures");
                            Tilc::Data::TDataProcessor::CopyPictures("products", Id, NewPictures, "pictures");
                        }
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

int Tilc::Data::TDataProcessor::CopyPictures(const std::string_view TableName, const std::string_view ItemId, const std::string_view NewPictures, const std::string_view PictureType)
{
    Tilc::TExtString DbFnameSrc = Tilc::TExtString("./db/") + TableName.data() + "_pictures.db";
    Tilc::TExtString DbFnameDest = Tilc::TExtString("./db/") + TableName.data() + "_pictures_" + ItemId.data() + ".db";

    Tilc::Data::TDBSQLite DBSrc(DbFnameSrc.c_str());
    Tilc::Data::TDBDataRows Rows;

    int Counter{};
    // First get miniatures
    DBSrc.Select(std::format("SELECT name, data FROM {} ORDER BY id ASC", PictureType).c_str(), {}, {}, Rows);
    for (auto const& Row : Rows)
    {
        if (NewPictures.find(Row[0]) == std::string::npos) continue;
        else if (
            NewPictures == Row[0] ||
            NewPictures.find(Row[0] + ",") == 0 ||
            NewPictures.find("," + Row[0] + ",") != std::string::npos ||
            NewPictures.find("," + Row[0]) == NewPictures.length() - Row[0].length() - 1
            )
        {
            Tilc::Data::TDataDBResources::CreateSimpleDataDb(DbFnameDest.c_str(), PictureType.data());
            Tilc::Data::TDataDBResources::SetSimpleData(DbFnameDest.c_str(), PictureType.data(), Row[0].c_str(),
                Row[1].c_str(), Row[1].length());
            ++Counter;
        }
    }

    return Counter;
}
