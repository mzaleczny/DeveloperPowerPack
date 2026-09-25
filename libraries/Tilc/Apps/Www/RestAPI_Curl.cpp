#ifndef __EMSCRIPTEN__

#include "Tilc/Apps/Www/RestAPI.h"

Tilc::TExtString Tilc::Apps::Www::Delete(Tilc::TExtString Url, std::vector<int>& Ids)
{
    Tilc::TExtString ResultCode;
    Tilc::Net::THttp Http;
    Tilc::TExtString Json;
    Tilc::TExtString JsonItems;

    Json = R"##(
    {
        "Action": "Delete",
        "Ids": [{IDS}]
    })##";
    Json.StrReplace("{IDS}", Tilc::Implode(',', Ids));
    return Http.DoPost(std::move(Url), Json, {}, ResultCode);
}

#endif
