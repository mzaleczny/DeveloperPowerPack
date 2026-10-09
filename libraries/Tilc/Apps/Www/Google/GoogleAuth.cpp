#include <jwt-cpp/jwt.h>
#include <nlohmann/json.hpp>
#include "Tilc/Net/Http.h"
#include "Tilc/Apps/Www/Google/GoogleAuth.h"

DECLSPEC std::string Tilc::Google::get_google_public_key_by_kid(const std::string& jwks_response, const std::string& target_kid) {
    using json = nlohmann::json;
    auto parsed_jwks = json::parse(jwks_response);
    // 1. Przeszukujemy tablicę "keys" w poszukiwaniu odpowiedniego "kid"
    if (parsed_jwks.contains("keys") && parsed_jwks["keys"].is_array()) {
        for (const auto& key : parsed_jwks["keys"]) {
            if (key.contains("kid") && key["kid"] == target_kid) {
                // Znaleźliśmy właściwy klucz. Sprawdzamy, czy zawiera parametry RSA (n oraz e)
                if (key.contains("n") && key.contains("e")) {
                    // UWAGA: jwt-cpp potrzebuje surowego klucza w formacie PEM lub 
                    // konstrukcji bezpośrednio z parametrów RSA.
                    // Najnowsze wersje jwt-cpp pozwalają na weryfikację za pomocą wyciągniętych n i e.
                    return key.dump(); // Zwracamy pod-json tego konkretnego klucza
                }
            }
        }
    }
    throw std::runtime_error("Klucz o podanym kid nie został znaleziony w JWKS Google.");
}

Tilc::TUser Tilc::Google::VerifyGoogleToken(const std::string& Token, const std::string& ClientId)
{
    Tilc::TUser Result;
    try
    {
        using json = nlohmann::json;
        auto decoded = jwt::decode(Token);
        std::string token_kid = decoded.get_header_claim("kid").as_string();

        // 1. Sprawdzenie wydawcy (iss)
        auto iss = decoded.get_issuer();
        if (iss != "https://accounts.google.com" && iss != "accounts.google.com")
        {
            Result.error = "1";
            return Result;
        }

        // 2. Sprawdzenie odbiorcy (aud)
        if (!decoded.has_audience())
        {
            Result.error = "2";
            return Result;
        }
        auto audiences = decoded.get_audience();
        if (audiences.find(ClientId) == audiences.end())
        {
            Result.error = "3";
            return Result;
        }

        // Wyciągamy dane klucza dla konkretnego kid
        // Wykonanie zapytania HTTPS do Google JWKS
        // Adres URL: https://googleapis.com
        Tilc::TExtString ResultCode;
        Tilc::Net::THttp http;
        Tilc::TExtString jwks_response = http.DoGet("https://www.googleapis.com/oauth2/v3/certs", "", {}, ResultCode);
        if (ResultCode != "OK") 
        {
            throw std::runtime_error("Błąd połączenia. Nie udało się pobrać kluczy JWKS od Google. " + jwks_response);
        }
        std::string raw_key_json = get_google_public_key_by_kid(jwks_response, token_kid);
        auto key_obj = json::parse(raw_key_json);

        std::string modulus = key_obj["n"];
        std::string exponent = key_obj["e"];

        // Weryfikacja tokenu przy użyciu wyekstrahowanych parametrów RSA
        auto verifier = jwt::verify()
            .allow_algorithm(jwt::algorithm::rs256(
                jwt::helper::create_public_key_from_rsa_components(modulus, exponent)
            ))
            .with_issuer("https://accounts.google.com");

        verifier.verify(decoded);
        
        // Z wyczytanego tokenu możemy teraz pobrać dane użytkownika:
        Tilc::TUser u;
        u.email = decoded.get_payload_claim("email").as_string();
        u.sub = decoded.get_payload_claim("sub").as_string();
        u.name = decoded.get_payload_claim("name").as_string();
        u.picture = decoded.get_payload_claim("picture").as_string();

        return u;
    }
    catch (const std::exception& e)
    {
        // Błąd weryfikacji sygnatury lub wygaśnięcie tokenu
        Result.error = e.what();
        return Result;
    }
}
