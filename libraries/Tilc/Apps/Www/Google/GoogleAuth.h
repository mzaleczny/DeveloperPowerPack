#pragma once

#include "Tilc/DllGlobals.h"
#include "Tilc/Tilc.h"
#include "Tilc/Apps/Www/RequestHandler.h"

namespace Tilc
{
    namespace Google
    {
        struct DECLSPEC TGoogleUser
        {
            Tilc::TExtString Email;
            Tilc::TExtString Sub; // unikalny ID Google
            Tilc::TExtString Name;
            Tilc::TExtString Picture;
            Tilc::TExtString Error;
        };

        DECLSPEC std::string get_google_public_key_by_kid(const std::string& jwks_response, const std::string& target_kid);
        DECLSPEC TGoogleUser VerifyGoogleToken(const std::string& Token, const std::string& ClientId);
    }
}
