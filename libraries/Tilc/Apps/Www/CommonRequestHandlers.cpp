#include "Tilc/Apps/Www/CommonRequestHandlers.h"
#include "Tilc/Apps/Www/Google/GoogleAuth.h"
#include "Tilc/Data/DataDbResources.h"
#include "Tilc/Net/Http.h"
#include "Tilc/Utils/Base64.h"
#include "Tilc/Utils/Time.h"

DECLSPEC void Tilc::Apps::Www::Return_404_NotFound(Tilc::Apps::Www::TRequestHandler& rh, const std::string_view Body404)
{
    rh.Headers.push_back("Status: 404 Not Found");
    rh.ContentType = "text/html; charset=utf-8";
    rh.OutputHeaders();

    if (Body404.length() == 0)
    {
        rh << "<!DOCTYPE html>";
        rh << "<html><head><title>404 Nie znaleziono</title></head>";
        rh << "<body><h1>404 Not Found</h1><p>Szukana strona nie istnieje.</p></body></html>";
    }
    else
    {
        rh << Body404.data();
    }
}

DECLSPEC void Tilc::Apps::Www::GoogleLogin(Tilc::Apps::Www::TRequestHandler& rh, Tilc::TExtString Url)
{
    Tilc::TExtString GoogleAuthToken = rh.PostVars["token"];
    /*
    rh << "<ol>\n";
    char** env = rh.GetRequest()->envp;
    if (env != nullptr)
    {
        for (; *env != nullptr; ++env)
        {
            rh << "<li><code>" << *env << "</code></li>\n";
        }
    }
    else
    {
        rh << "<li>Brak zmiennych przesłanych przez serwer.</li>\n";
    }
    rh << "</ol>\n";
    */
    //rh << "*" << GoogleAuthToken << "*";


    // Weryfikacja tokenu
    //rh.Log(GetCurrentDateTime(), "    Call VerifyGoogleToken for " + GoogleAuthToken + " :: " + Tilc::GoogleClientId + "\n");
    Tilc::TUser User = Tilc::Google::VerifyGoogleToken(GoogleAuthToken, Tilc::GoogleClientId);
    Tilc::TExtString DbFname = rh.ApplicationRootDir + "/db/sessions.db";
    //rh.Log(GetCurrentDateTime(), "Error: " + User.error + "\n");
    //rh << "Error: " << User.error << "\n";
    //rh << GoogleAuthToken << "\n";
    //rh << GoogleClientId << "\n";
    if (User.error.empty())
    {
        rh.ContentType = "application/json";
        rh.OutputHeaders();
        Tilc::TExtString JsonUser = "{"
                R"("email": "[EMAIL]",)"
                R"("sub": "[SUB]",)"
                R"("name": "[NAME]",)"
                R"("picture": "[PICTURE]",)"
                R"("token": "[TOKEN]",)"
                R"("picture_blob": "[PICTURE_BLOB]")"
            "}";
        JsonUser.StrReplace("[EMAIL]", User.email);
        JsonUser.StrReplace("[SUB]", User.sub);
        JsonUser.StrReplace("[NAME]", User.name);
        JsonUser.StrReplace("[PICTURE]", User.picture);
        JsonUser.StrReplace("[TOKEN]", GoogleAuthToken);

        //rh.Log(GetCurrentDateTime(), "CreateSimpleDataDb: " + DbFname + "\n");
        Tilc::Data::TDataDBResources::CreateSimpleDataDb(DbFname.c_str(), "picture_blobs");
        //rh.Log(GetCurrentDateTime(), "AFTER CreateSimpleDataDb: " + DbFname + "\n");
        auto [id, data, modified] = Tilc::Data::TDataDBResources::GetSimpleData(DbFname.c_str(), "picture_blobs", User.email.c_str());
        //rh.Log(GetCurrentDateTime(), "GetSimpleData: " + std::to_string(id) + ", " + data + "\n");
        if (id < 1 || data.length() < 1)
        {
            Tilc::Net::THttp Http;
            Tilc::TExtString ResultCode, DataBase64;
            //rh.Log(GetCurrentDateTime(), "Http.DoGet(User.picture: " + User.picture + "\n");
            data = Http.DoGet(User.picture, "", {}, ResultCode);
            if (!data.empty())
            {
                Tilc::TBase64::Encode(reinterpret_cast<const unsigned char*>(data.c_str()), static_cast<long>(data.length()), DataBase64);
                //rh.Log(GetCurrentDateTime(), "AFTER Tilc::TBase64::Encode - SetSimpleData: " + DbFname + "\n");
                Tilc::Data::TDataDBResources::SetSimpleData(DbFname.c_str(), "picture_blobs", User.email.c_str(),
                    DataBase64.c_str(), DataBase64.length());
                //rh.Log(GetCurrentDateTime(), "AFTER\n");
                data = std::move(DataBase64);
                //rh.Log(GetCurrentDateTime(), "AFTER std::move\n");
            }
        }
        JsonUser.StrReplace("[PICTURE_BLOB]", data);

        //rh.Log(GetCurrentDateTime(), "CreateSimpleDataDb logins\n");
        Tilc::Data::TDataDBResources::CreateSimpleDataDb(DbFname.c_str(), "logins");
        //rh.Log(GetCurrentDateTime(), "SetSimpleData logins\n");
        Tilc::Data::TDataDBResources::SetSimpleData(DbFname.c_str(), "logins", User.email.c_str(),
            JsonUser.c_str(), JsonUser.length());
        //rh.Log(GetCurrentDateTime(), "AFTER SetSimpleData logins\n");

        Tilc::TExtString Result = R"({"status": "ok", "message": "Zalogowano pomyślnie", "user": [USER]})";
        Result.StrReplace("[USER]", JsonUser);
        //rh.Log(GetCurrentDateTime(), "Result: " + Result + "\n");
        rh << Result;
    }
    else
    {
        rh.ContentType = "application/json";
        rh.Headers.push_back("Status: 401 Unauthorized");
        Tilc::TExtString Result = R"({"status": "error", "message": "Niepoprawny token"})";
        rh << Result;
    }
}

DECLSPEC void Tilc::Apps::Www::CheckGoogleLogin(Tilc::Apps::Www::TRequestHandler& rh, Tilc::TExtString Url)
{
    rh.ContentType = "application/json";
    Tilc::TExtString Result;
    Tilc::TExtString DbFname = rh.ApplicationRootDir + "/db/sessions.db";
    auto [id, data, modified] = Tilc::Data::TDataDBResources::GetSimpleData(DbFname.c_str(), "logins", rh.PostVars["email"].c_str());
    if (id > 0)
    {
        Result = R"({"status": "Logged In", "modified": "[MODIFIED]", "user": [USER]})";
        auto time = std::chrono::system_clock::to_time_t(modified);
        std::ostringstream oss;
        oss << std::put_time(std::localtime(&time), "%F %T");
        Result.StrReplace("[MODIFIED]",oss.str());
        Result.StrReplace("[USER]", data);
    }
    else
    {
        Result = R"({"status": "Not Logged In"})";
    }
    rh << Result;
}
