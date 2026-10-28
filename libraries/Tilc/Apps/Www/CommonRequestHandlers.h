#pragma once

#include "Tilc/Tilc.h"
#include "Tilc/Apps/Www/RequestHandler.h"
#include "Tilc/Data/DbMySQL.h"
#include "Tilc/Data/DataProcessor.h"
#include "Tilc/Data/DataDbResources.h"
#include "Tilc/Commerce/Shop.h"
#include "Tilc/Utils/Base64.h"
#include <chrono>

namespace Tilc
{
    namespace Apps
    {
        namespace Www
        {
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
                    PrintItemsListWithPictures(rh, DB, Item, Tilc::TExtString("SELECT %s FROM ") + Item.TableName + " WHERE id=" + id, Item.TableName, id);
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

            void GoogleLogin(Tilc::Apps::Www::TRequestHandler& rh, Tilc::TExtString Url);
            void CheckGoogleLogin(Tilc::Apps::Www::TRequestHandler& rh, Tilc::TExtString Url);

        }
    }
}
