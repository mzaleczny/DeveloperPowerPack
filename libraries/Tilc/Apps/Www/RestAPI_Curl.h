#pragma once

#ifndef __EMSCRIPTEN__

#include "Tilc/Utils/ExtString.h"
#include "Tilc/Net/Http.h"
#include "Tilc/Commerce/Shop.h"
#include <iostream>
#include "Tilc/Utils/JsonParser.h"

namespace Tilc
{
    namespace Apps
    {
        namespace Www
        {
            template <typename T>
            inline T Fetch(Tilc::TExtString Url, Tilc::TExtString Id)
            {
                Tilc::TExtString ResultCode;
                Tilc::Net::THttp Http;
                Tilc::TExtString JsonItem = Http.DoPost(std::move(Url), Id, {}, ResultCode);
                T Item;
                Item.FromJson(JsonItem);
                return Item;
            }


            inline Tilc::TExtString Fetch(Tilc::TExtString Url)
            {
                Tilc::TExtString ResultCode;
                Tilc::Net::THttp Http;
                return Http.DoGet(std::move(Url), "", {}, ResultCode);
            }


            template <typename T>
            Tilc::TExtString Save(Tilc::TExtString Url, std::vector<T>& Items)
            {
                Tilc::TExtString ResultCode;
                Tilc::Net::THttp Http;
                Tilc::TExtString Json;
                Tilc::TExtString JsonItems;

                for (size_t i = 0; i < Items.size(); ++i)
                {
                    if (i > 0) JsonItems += ",";
                    JsonItems += Items[i].ToJson();
                }
                Json = R"##(
                {
                    "Action": "Save",
                    "Items": [{ITEMS}]
                })##";
                Json.StrReplace("{ITEMS}", JsonItems);
                return Http.DoPost(std::move(Url), Json, {}, ResultCode);
            }


            Tilc::TExtString Delete(Tilc::TExtString Url, std::vector<int>& Ids);
        }
    }
}
#endif
