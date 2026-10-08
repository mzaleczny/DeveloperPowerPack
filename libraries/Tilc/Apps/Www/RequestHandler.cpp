#include "Tilc/Apps/Www/RequestHandler.h"
#include "Tilc/Apps/Www/WwwApp.h"
#include "Tilc/Globals.h"

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
    //os << "Url: " << Url << "<br/>";
    auto Found = m_RequestHandlers.find(Url);
    if (Found != m_RequestHandlers.end())
    {
        Found->second(*this, Url);
    }

    // Call below guarantees OutputHeaders even if not << opertor was called. Because by default OutputHeaders is called
    // before first call of <<.
    OutputHeaders();
}

void Tilc::Apps::Www::TRequestHandler::ExtractVariablesFromQueryString(const Tilc::TExtString QueryString, std::unordered_map<std::string, Tilc::TExtString>& Map)
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
    ContentLength = std::clamp(atoi(FCGX_GetParam("CONTENT_LENGTH", request->envp)), 0, 20_MB);
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
