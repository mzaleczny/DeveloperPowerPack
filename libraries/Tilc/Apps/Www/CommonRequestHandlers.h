#pragma once

#include "Tilc/Tilc.h"
#include "Tilc/Apps/Www/RequestHandler.h"
#include "Tilc/Data/DbMySQL.h"
#include "Tilc/Data/DbSQLite.h"
#include "Tilc/Data/DataProcessor.h"
#include "Tilc/Data/DataDbResources.h"
#include "Tilc/Commerce/Shop.h"
#include "Tilc/Utils/Base64.h"
#include <chrono>
#include <cctype>

namespace Tilc
{
    namespace Apps
    {
        namespace Www
        {
            DECLSPEC void Return_404_NotFound(Tilc::Apps::Www::TRequestHandler& rh, const std::string_view Body404 = "");

            template <typename T>
            void PrintItemsList(Tilc::Apps::Www::TRequestHandler& rh, Tilc::Data::TDBMySQL& DB, T& Item, const Tilc::TExtString& SelectSQL)
            {
                Tilc::Data::TDataProcessor::PrintList(rh.os, DB, SelectSQL.c_str(), Item.GetAllFieldsNamesList());
            }

            template <typename T>
            void FetchItem(Tilc::Apps::Www::TRequestHandler& rh, const Tilc::TExtString Url)
            {
                rh.ContentType = "application/json";
                rh.OutputHeaders();

                // Id is specified in body
                if (rh.Body.length() > 0)
                {
                    Tilc::TExtString id = rh.Body.substr(0, 12);

                    Tilc::Data::TDBMySQL DB(Tilc::DbConnectionParams.DBHost, Tilc::DbConnectionParams.DBName, Tilc::DbConnectionParams.DBUser, Tilc::DbConnectionParams.DBPasswd, Tilc::DbConnectionParams.DBSocket);
                    T Item;
                    Tilc::Data::TDataProcessor::PrintItemWithPictures(rh.os, DB, (Tilc::TExtString("SELECT %s FROM ") + Item.TableName + " WHERE id=" + id).c_str(),
                        Item.GetAllFieldsNamesList(), Item.TableName, id);
                }
            }

            template <typename T>
            void FetchItemsList(Tilc::Apps::Www::TRequestHandler& rh, const Tilc::TExtString Url)
            {
                rh.ContentType = "application/json";
                rh.OutputHeaders();

                Tilc::Data::TDBMySQL DB(Tilc::DbConnectionParams.DBHost, Tilc::DbConnectionParams.DBName, Tilc::DbConnectionParams.DBUser, Tilc::DbConnectionParams.DBPasswd, Tilc::DbConnectionParams.DBSocket);
                T Item;
                PrintItemsList(rh, DB, Item, Tilc::TExtString("SELECT %s FROM ") + Item.TableName);
            }

            template <typename T>
            void GetImage(Tilc::Apps::Www::TRequestHandler& rh, const Tilc::TExtString Url)
            {
                // sprawdzamy poprawnosc zmiennej name
                if (rh.GetVars.find("name") == rh.GetVars.end())
                {
                    Tilc::Apps::Www::Return_404_NotFound(rh);
                    return;
                }
                else
                {
                    for (auto const& ch : rh.GetVars["name"])
                    {
                        if (!std::isalnum(ch) && ch != '_' && ch != '-')
                        {
                            Tilc::Apps::Www::Return_404_NotFound(rh);
                            return;
                        }
                    }
                }

                // sprawdzamy poprawnosc zmiennej name
                if (rh.GetVars.find("id") == rh.GetVars.end())
                {
                    Tilc::Apps::Www::Return_404_NotFound(rh);
                    return;
                }
                else
                {
                    if (!rh.GetVars["id"].isInt())
                    {
                        Tilc::Apps::Www::Return_404_NotFound(rh);
                        return;
                    }
                }

                T Item;
                Tilc::TExtString DbFname = rh.ApplicationRootDir + "/db/" + Item.TableName + "_pictures_" + rh.GetVars["id"] + ".db";
                sqlite3* db;
                sqlite3_open(DbFname.c_str(), &db);

                const char* ImageType = "pictures";
                if (rh.GetVars["type"] == "miniatures")
                {
                    ImageType = "miniatures";
                }
                Tilc::TExtString SqlQuery = std::format("SELECT data FROM {} WHERE name=?;", ImageType);
                sqlite3_exec(db, SqlQuery.c_str(), nullptr, nullptr, nullptr);

                sqlite3_stmt* stmt;
                int rc = sqlite3_prepare_v2(db, SqlQuery.c_str(), -1, &stmt, nullptr);
                if (rc != SQLITE_OK)
                {
                    sqlite3_close(db);
                    Tilc::Apps::Www::Return_404_NotFound(rh);
                    return;
                }
                sqlite3_bind_text(stmt, 1, rh.GetVars["name"].c_str(), -1, SQLITE_STATIC);
                if (sqlite3_step(stmt) == SQLITE_ROW)
                {
                    rh.ContentType = "image/webp";
                    rh.Headers.push_back(std::format("Content-Length: {}", sqlite3_column_bytes(stmt, 0)));
                    rh.OutputHeaders();
                    rh << static_cast<const char*>(sqlite3_column_blob(stmt, 0));
                }
                sqlite3_finalize(stmt);
                sqlite3_close(db);
            }

            template <typename T>
            void SaveItems(Tilc::Apps::Www::TRequestHandler& rh, const Tilc::TExtString Url)
            {
                rh.ContentType = "application/json";
                rh.OutputHeaders();
                Tilc::Data::TDBMySQL DB(Tilc::DbConnectionParams.DBHost, Tilc::DbConnectionParams.DBName, Tilc::DbConnectionParams.DBUser, Tilc::DbConnectionParams.DBPasswd, Tilc::DbConnectionParams.DBSocket);
                T Item;

                if (rh.Body.length() > 0)
                {
                    Tilc::Data::TDataProcessor::SaveItems(rh.os, DB, Item.TableName.c_str(), rh.Body);
                }

                PrintItemsList(rh, DB, Item, Tilc::TExtString("SELECT %s FROM ") + Item.TableName);
            }

            template <typename T>
            void DeleteItems(Tilc::Apps::Www::TRequestHandler& rh, const Tilc::TExtString Url)
            {
                rh.ContentType = "application/json";
                rh.OutputHeaders();

                Tilc::Data::TDBMySQL DB(Tilc::DbConnectionParams.DBHost, Tilc::DbConnectionParams.DBName, Tilc::DbConnectionParams.DBUser, Tilc::DbConnectionParams.DBPasswd, Tilc::DbConnectionParams.DBSocket);
                T Item;

                if (rh.Body.length() > 0)
                {
                    Tilc::Data::TDataProcessor::DeleteItems(rh.os, DB,
                        ("DELETE FROM " + Item.TableName + " WHERE id IN (%s)").c_str(),
                        rh.Body
                    );
                }

                PrintItemsList(rh, DB, Item, Tilc::TExtString("SELECT %s FROM ") + Item.TableName);
            }

            template <typename T>
            void UploadImage(Tilc::Apps::Www::TRequestHandler& rh, Tilc::TExtString Url)
            {
                T Item;
                auto now = std::chrono::system_clock::now();
                auto duration = now.time_since_epoch();
                auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();

                Tilc::TExtString DbFname = rh.ApplicationRootDir + "/db/" + Item.TableName + "_pictures.db";
                Tilc::TExtString DecodedBlobPicture, DecodedBlobScaledPicture;
                Tilc::TExtString ItemId = rh.PostVars["ItemId"];

                Tilc::TExtString ClientVarName = rh.PostVars["ClientPicVarName"];
                Tilc::TExtString ServerVarName = "Pic_" + ItemId + "_" + std::to_string(milliseconds);

                Tilc::TBase64::Decode(rh.PostVars["BlobPicture"], DecodedBlobPicture);
                Tilc::TBase64::Decode(rh.PostVars["BlobScaledPicture"], DecodedBlobScaledPicture);

                Tilc::Data::TDataDBResources::CreateSimpleDataDb(DbFname.c_str(), "pictures");
                Tilc::Data::TDataDBResources::CreateSimpleDataDb(DbFname.c_str(), "miniatures");

                Tilc::Data::TDataDBResources::SetSimpleData(DbFname.c_str(), "pictures", ServerVarName.c_str(),
                    DecodedBlobPicture.c_str(), DecodedBlobPicture.length());
                Tilc::Data::TDataDBResources::SetSimpleData(DbFname.c_str(), "miniatures", ServerVarName.c_str(),
                    DecodedBlobScaledPicture.c_str(), DecodedBlobScaledPicture.length());

                rh << "OK:" << ServerVarName << ":" << ClientVarName;
            }

            template <typename T>
            void GetPictures(Tilc::Apps::Www::TRequestHandler& rh, Tilc::TExtString Url)
            {
                T Item;
                Tilc::TExtString ItemId = rh.GetVars["ItemId"];
                Tilc::TExtString DbFname = rh.ApplicationRootDir + "/db/" + Item.TableName + "_pictures_" + ItemId + ".db";
                Tilc::Data::TDBSQLite DB(DbFname.c_str());
                Tilc::Data::TDBDataRows Rows;

                // First get miniatures
                DB.Select("SELECT name, data FROM miniatures ORDER BY id ASC", {}, {}, Rows);
                bool First = true;
                for (auto const& Row : Rows)
                {
                    if (!First)
                    {
                        std::cout << "&";
                    }
                    else
                    {
                        First = false;
                    }
                    std::cout << Row[0] << "_mini=" << Row[1];
                }

                // Next get original pictures
                Rows.clear();
                DB.Select("SELECT name, data FROM pictures ORDER BY id ASC", {}, {}, Rows);
                for (auto const& Row : Rows)
                {
                    std::cout << "&";
                    std::cout << Row[0] << "_pic=" << Row[1];
                }
            }

            DECLSPEC void GoogleLogin(Tilc::Apps::Www::TRequestHandler& rh, Tilc::TExtString Url);
            DECLSPEC void CheckGoogleLogin(Tilc::Apps::Www::TRequestHandler& rh, Tilc::TExtString Url);

        }
    }
}
