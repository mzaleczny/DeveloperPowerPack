#ifndef __EMSCRIPTEN__

#include "Tilc/Apps/Www/Api/RestAPI.h"

Tilc::TExtString Tilc::Apps::Www::Delete(Tilc::TExtString Url, std::vector<int>& Ids, Tilc::TExtString LoggedUser)
{
    Tilc::TExtString ResultCode;
    Tilc::Net::THttp Http;
    Tilc::TExtString Json;
    Tilc::TExtString JsonItems;

    Json = R"##(
    {
        "Action": "Delete",
        "LoggedUser": "{LOGGED_USER}",
        "Ids": [{IDS}]
    })##";
    Json.StrReplace("{IDS}", Tilc::Implode(',', Ids));
    Json.StrReplace("{LOGGED_USER}", LoggedUser);
    return Http.DoPost(std::move(Url), Json, {}, ResultCode);
}

#endif
