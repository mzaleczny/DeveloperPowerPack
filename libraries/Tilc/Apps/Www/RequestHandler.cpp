#include "Tilc/Apps/Www/RequestHandler.h"
#include "Tilc/Apps/Www/WwwApp.h"

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
    Bearer = FCGX_GetParam("HTTP_BEARER", request->envp) ? FCGX_GetParam("HTTP_BEARER", request->envp) : "";
}

void Tilc::Apps::Www::TRequestHandler::Cleanup()
{
}

void Tilc::Apps::Www::TRequestHandler::OutputHeaders()
{
	std::for_each(Headers.begin(), Headers.end(), [this](Tilc::TExtString hdr) {
		os << hdr << "\r\n";
	});
    os << "\r\n";
}

void Tilc::Apps::Www::TRequestHandler::HandleRequest()
{
    Headers.push_back("Content-type: text/html; charset=utf-8");
    if (Application)
    {
        Headers.push_back("Access-Control-Allow-Origin: " + Application->GetAllowOrigin());
    }
//    if (UserAgent != "Teacher Application Curl Http Client")
//    {
//        return;
//    }

    if (Application && ApplicationRootDir.find(Application->GetAllowedRootDir()) != 0)
    {
        return;
    }

    OutputHeaders();
    // os << "OK";
//    for (size_t i = 0; i < UriParts.size(); ++i)
//    {
//        os << "UriParts[" << i << "] = " << UriParts[i] << "<br/>";
//    }
    switch (RequestMethod)
    {
        case ERequestMethod::ERM_GET:
            break;
        case ERequestMethod::ERM_POST:
            ReadPostData();
            break;
        default:
           break;
    }
    
    Tilc::TExtString Url = "/" + Tilc::Implode('/', UriParts);
    os << "Url: " << Url << "<br/>";
    auto Found = m_RequestHandlers.find(Url);
    if (Found != m_RequestHandlers.end())
    {
        Found->second(*this, Url);
    }
}

void Tilc::Apps::Www::TRequestHandler::ReadPostData()
{
    ContentLength = atoi(FCGX_GetParam("CONTENT_LENGTH", request->envp));
    Body.assign( (std::istreambuf_iterator<char>(is)),
                 (std::istreambuf_iterator<char>())
    );

    // check if body is ascii-encoded or if it has non printable (binary) characters. If the first case
    // then do urldecode on it
    /*
    bool IsAscii = true;
    for (int i = 0; i < Body.size(); ++i)
    {
        if (!isprint(Body[i]))
        {
            IsAscii = false;
            break;
        }
    }

    if (IsAscii)
    {
        Body = urldecode(Body);
    }
    else
    {

    }
    
    PostVars.clear();
    std::vector<Tilc::TExtString> Pairs, Variables;
    Body.Explode('&', Pairs);
    for (size_t i = 0; i < Pairs.size(); ++i)
    {
        Pairs[i].Explode('=', Variables);
        if (Variables.size() == 2)
        {
            PostVars.emplace(Variables[0], Variables[1]);
        }
    }
    */
}

Tilc::Apps::Www::TRequestHandler& Tilc::Apps::Www::TRequestHandler::operator<<(const std::string& val)
{
    os << val.c_str();
    return *this;
}

Tilc::Apps::Www::TRequestHandler& Tilc::Apps::Www::TRequestHandler::operator<<(const char* val)
{
    os << val;
    return *this;
}

Tilc::Apps::Www::TRequestHandler& Tilc::Apps::Www::TRequestHandler::operator<<(int val)
{
    os << std::to_string(val);
    return *this;
}
