#include "Tilc/Apps/Www/RequestHandler.h"
#include "Tilc/Apps/Www/WwwApp.h"
#include "Tilc/Apps/Www/Google/GoogleAuth.h"
#include "Tilc/Commerce/Shop.h"
#include "Tilc/Utils/JsonParser.h"
#include "Tilc/Utils/StdObject.h"
#include "Tilc/Data/DataDbResources.h"
#include "Tilc/Globals.h"
#include <chrono>

long long int Tilc::Apps::Www::TRequestHandler::Count = 0;

Tilc::Apps::Www::TRequestHandler::TRequestHandler(FCGX_Request* Request, TRoutes& Routes)
    : request(Request),
        cin_fcgi_streambuf{Request->in},
        cout_fcgi_streambuf{Request->out},
        cerr_fcgi_streambuf{Request->err},
        os{&cout_fcgi_streambuf},
        errs{&cerr_fcgi_streambuf},
        is{&cin_fcgi_streambuf},
        m_RequestHandlers(Routes)
{
    Init();
}

Tilc::Apps::Www::TRequestHandler::~TRequestHandler()
{
    Cleanup();
}

void Tilc::Apps::Www::TRequestHandler::Init()
{
    QueryString = FCGX_GetParam("QUERY_STRING", request->envp) ? FCGX_GetParam("QUERY_STRING", request->envp) : "";
    ScriptName = FCGX_GetParam("SCRIPT_NAME", request->envp) ? FCGX_GetParam("SCRIPT_NAME", request->envp) : "";
    RequestUri = FCGX_GetParam("REQUEST_URI", request->envp) ? FCGX_GetParam("REQUEST_URI", request->envp) : "";
    Referer = FCGX_GetParam("HTTP_REFERER", request->envp) ? FCGX_GetParam("HTTP_REFERER", request->envp) : "";
    UserAgent = FCGX_GetParam("HTTP_USER_AGENT", request->envp) ? FCGX_GetParam("HTTP_USER_AGENT", request->envp) : "";
    ScriptFileName = FCGX_GetParam("SCRIPT_FILENAME", request->envp) ? FCGX_GetParam("SCRIPT_FILENAME", request->envp) : "";
    ApplicationRootDir = ScriptFileName;
    ApplicationRootDir.UnshiftRight('/');
    Log.SetFile(ApplicationRootDir + "/log.txt");
    
    RequestMethod = ToRequestMethod(FCGX_GetParam("REQUEST_METHOD", request->envp) ? FCGX_GetParam("REQUEST_METHOD", request->envp) : "");
    if (RequestUri.length() > 0)
    {
        if (size_t pos = RequestUri.find("?"); pos != std::string::npos)
        {
            RequestUri = RequestUri.substr(0, pos);
        }
        RequestUri.Explode('/', UriParts);
        // remove all empy UriParts from the beginning
        while (UriParts.size() > 0 && UriParts[0].length() == 0)
        {
            UriParts.erase(UriParts.cbegin());
        }
        // if first elem now is "tasks", then we ignore it
        if (UriParts.size() > 0 && Tilc::Apps::Www::Application && UriParts[0] == Tilc::Apps::Www::Application->GetAppSlug())
        {
            UriParts.erase(UriParts.cbegin());
        }
    }
    // if UriParts[0] is a language "pl" or "en", then set Lang variable and remove first elem from UriParts
    // Default one is "pl"
    Lang = "pl";
    if (Tilc::Apps::Www::Application && UriParts.size() > 0)
    {
        const std::vector<Tilc::TExtString>& AvailableLangs = Application->GetLanguages();
        if (std::find(AvailableLangs.begin(), AvailableLangs.end(), UriParts[0]) != AvailableLangs.end())
        {
            Lang = UriParts[0];
            UriParts.erase(UriParts.cbegin());
        }
    }

    // Get client request headers
    Bearer = FCGX_GetParam("Authorization", request->envp) ? FCGX_GetParam("Authorization", request->envp) : "";
    if (Bearer.empty())
    {
        Bearer = FCGX_GetParam("HTTP_BEARER", request->envp) ? FCGX_GetParam("HTTP_BEARER", request->envp) : "";
    }
    if (auto pos = Bearer.find("Bearer "); pos != std::string::npos)
    {
        Bearer = Bearer.substr(0, pos + 7);
    }
    LoggedUser = FCGX_GetParam("LoggedUser", request->envp) ? FCGX_GetParam("LoggedUser", request->envp) : "";
}

void Tilc::Apps::Www::TRequestHandler::Cleanup()
{
}

void Tilc::Apps::Www::TRequestHandler::OutputHeaders()
{
    if (!m_HeadersSent)
    {
        Headers.push_back("Content-type: " + ContentType + "; charset=utf-8");
        if (Application)
        {
            Headers.push_back("Access-Control-Allow-Origin: " + Application->GetAllowOrigin());
        }

        m_HeadersSent = true;
        std::for_each(Headers.begin(), Headers.end(), [this](Tilc::TExtString hdr) {
            os << hdr << "\r\n";
        });
        os << "\r\n";
    }
}

void Tilc::Apps::Www::TRequestHandler::HandleRequest()
{
//    if (UserAgent != "Teacher Application Curl Http Client")
//    {
//        return;
//    }

    if (Application && ApplicationRootDir.find(Application->GetAllowedRootDir()) != 0)
    {
        OutputHeaders();
        return;
    }

    // os << "OK";
//    for (size_t i = 0; i < UriParts.size(); ++i)
//    {
//        os << "UriParts[" << i << "] = " << UriParts[i] << "<br/>";
//    }
    switch (RequestMethod)
    {
        case ERequestMethod::ERM_GET:
            ReadGetData();
            break;
        case ERequestMethod::ERM_POST:
            ReadPostData();
            break;
        default:
            break;
    }
    
    Tilc::TExtString Url = "/" + Tilc::Implode('/', UriParts);
    Tilc::TExtString Result;
    //os << "Url: " << Url << "<br/>";
    auto Found = m_RequestHandlers.find(Url);
    if (Found != m_RequestHandlers.end())
    {
        // jeśli Url zawiera słowo 'save' lub 'delete', to chronimy dostęp do tej funkcji mechanizmem logowania
        // aby to zadziałało w nagłówku trzeba przesłać nazwę/email zalogowanego użytkownika
        if (Url.find("/save") == std::string::npos && Url.find("/delete") == std::string::npos)
        {
            // Do tej trasy dostep nie jest chroniony zalogowaniem, więc wywołujemy obsługe tej trasy
            Found->second(*this, Url);
        }
        else
        {
            if (LoggedUser.empty())
            {
                Result = R"({"status": "Not Logged In"})";
            }
            else
            {
                // Sprawdzamy czy user jest zalogowany
                Tilc::TExtString DbFname = ApplicationRootDir + "/db/sessions.db";
                Tilc::TExtString Token;
                auto [id, data, modified] = Tilc::Data::TDataDBResources::GetSimpleData(DbFname.c_str(), "logins", LoggedUser.c_str());
                if (id > 0 && std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - modified).count() < 15 * 60)
                {
                    //std::cout << ResultJson << std::endl;
                    Tilc::TJsonParser parser;
                    Tilc::TStdObject* Json = parser.parse(data);
                    if (Json)
                    {
                        if (Json->getAsObject("root"))
                        {
                            Tilc::TStdObject* User = Json->getAsObject("root")->getAsObject("user");
                            if (User)
                            {
                                Token = User->getAsString("token");
                            }
                        }
                        delete Json;
                    }
                    if (!Token.empty())
                    {
                        if (Tilc::TUser User = Tilc::Google::VerifyGoogleToken(Token, GoogleClientId); User.error.empty())
                        {
                            // Jesteśmy poprawnie zalogowani do zabezpieczonego urla, więc wywołujemy funkcję obsługi tej trasy
                            Found->second(*this, Url);
                        }
                    }
                }
                else
                {
                    Result = R"({"status": "Not Logged In"})";
                }
            }
        }
    }

    // Call below guarantees OutputHeaders even if not << opertor was called. Because by default OutputHeaders is called
    // before first call of <<.
    OutputHeaders();
    if (!Result.empty())
    {
        os << Result;
    }
}

void Tilc::Apps::Www::TRequestHandler::ExtractVariablesFromQueryString(Tilc::TExtString& QueryString, std::unordered_map<Tilc::TExtString, Tilc::TExtString>& Map)
{
    Map.clear();
    if (QueryString.empty())
    {
        return;
    }
    std::vector<Tilc::TExtString> VariablesPairs, Var;
    QueryString.Explode('&', VariablesPairs);
    for (auto pair : VariablesPairs)
    {
        pair.Explode('=', Var);
        if (Var.size() == 2)
        {
            Map[Var[0]] = Var[1];
        }
    }
}

void Tilc::Apps::Www::TRequestHandler::ReadGetData()
{
    ExtractVariablesFromQueryString(QueryString, GetVars);
}

void Tilc::Apps::Www::TRequestHandler::ReadPostData()
{
    using namespace CompUnits;
    // Content-Length may be max 20MB
    ContentLength = std::clamp(atoi(FCGX_GetParam("CONTENT_LENGTH", request->envp)), 0, static_cast<int>(20_MB));
    Body.assign( (std::istreambuf_iterator<char>(is)),
                 (std::istreambuf_iterator<char>())
    );
    ExtractVariablesFromQueryString(Body, PostVars);
}

Tilc::Apps::Www::TRequestHandler& Tilc::Apps::Www::TRequestHandler::operator<<(const std::string& val)
{
    if (!m_HeadersSent)
    {
        OutputHeaders();
    }
    os << val.c_str();
    return *this;
}

Tilc::Apps::Www::TRequestHandler& Tilc::Apps::Www::TRequestHandler::operator<<(const char* val)
{
    if (!m_HeadersSent)
    {
        OutputHeaders();
    }
    os << val;
    return *this;
}

Tilc::Apps::Www::TRequestHandler& Tilc::Apps::Www::TRequestHandler::operator<<(int val)
{
    if (!m_HeadersSent)
    {
        OutputHeaders();
    }
    os << std::to_string(val);
    return *this;
}
