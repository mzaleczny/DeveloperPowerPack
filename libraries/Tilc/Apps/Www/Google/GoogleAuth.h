#pragma once

#include "Tilc/DllGlobals.h"
#include "Tilc/Tilc.h"
#include "Tilc/Commerce/Shop.h"
#include "Tilc/Apps/Www/RequestHandler.h"

namespace Tilc
{
    namespace Google
    {
        DECLSPEC std::string get_google_public_key_by_kid(const std::string& jwks_response, const std::string& target_kid);
        DECLSPEC Tilc::TUser VerifyGoogleToken(const std::string& Token, const std::string& ClientId);
    }
}
