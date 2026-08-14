#include <cstring>
#include <cstdio>
#include "Tilc/Apps/Www/WwwApp.h"
#include "Tilc/Apps/Www/RequestHandler.h"

std::string Tilc::Apps::Www::DbHost{"localhost"};
std::string Tilc::Apps::Www::DbName{"teacher"};
std::string Tilc::Apps::Www::DbUser{"teacher"};
std::string Tilc::Apps::Www::DbPasswd{"teacher"};
Tilc::Apps::Www::TRequestHandler* Tilc::Apps::Www::rh{};
Tilc::Apps::Www::TWwwApp* Tilc::Apps::Www::Application{};

Tilc::Apps::Www::ERequestMethod Tilc::Apps::Www::ToRequestMethod(std::string method)
{
    if (method == "GET")
    {
        return Tilc::Apps::Www::ERequestMethod::ERM_GET;
    }
    else if(method == "POST")
    {
        return Tilc::Apps::Www::ERequestMethod::ERM_POST;
    }
    else if(method == "HEAD")
    {
        return Tilc::Apps::Www::ERequestMethod::ERM_HEAD;
    }
    else if(method == "PUT")
    {
        return Tilc::Apps::Www::ERequestMethod::ERM_PUT;
    }
    else if(method == "DELETE")
    {
        return Tilc::Apps::Www::ERequestMethod::ERM_DELETE;
    }
    else
    {
        return Tilc::Apps::Www::ERequestMethod::ERM_INVALID;
    }
}

std::string Tilc::Apps::Www::urlencode(const std::string& Input)
{
	std::string Result;
	char	 ch;
	size_t	 sz, cur;
	int	 rc, cp;
    char* p;

	if (Input.empty())
		return "";

	/* 
	 * Leave three bytes per input byte for encoding. 
	 * This ensures we needn't range-check.
	 * First check whether our size overflows. 
	 * We do this here because we need our size!
	 */
	sz = Input.length() + 1;
    Result.reserve(sz * 3);

	for (cur = 0, cp = 0; (ch = Input[cp]) != '\0'; ++cp)
    {
		if (std::isalnum((unsigned char)ch) || ch == '-' || 
		    ch == '_' || ch == '.' || ch == '~')
        {
			Result += ch;
			continue;
		}
        else if (' ' == ch)
        {
			Result += '+';
			continue;
		}
		rc = std::snprintf(p + cur, 4, "%%%.2hhX", (unsigned char)ch);
		if (rc != 3)
        {
			return "";
		}
		cur += 3;
	}

	return Result;
}

std::string Tilc::Apps::Www::urldecode(const std::string& Input)
{
	char	 	 c, d;
	const char	*tail;
    std::string Result;
    int p;

	if (Input.empty())
		return "";

	/*
	 * Keep track of two positions: "p", where we'll write the
	 * decoded results, and "tail", which is from where we'll
	 * decode hex or copy data.
	 */
    p = 0;
	for (tail = Input.c_str(); (c = *tail) != '\0'; Result += c, ++p)
    {
		if (c != '%')
        {
			if (c == '+')
				c = ' ';
			tail++;
			continue;
		}

		/* 
		 * Read hex '%xy' as two unsigned chars "c" and "d" then
		 * combine them back into "c".
		 */
#ifdef __linux__
		if (std::sscanf(tail + 1, "%1hhx%1hhx", &d, &c) != 2 || (c |= d << 4) == '\0')
#elif __WIN32
        if (std::sscanf_s(tail + 1, "%1hhx%1hhx", &d, &c) != 2 || (c |= d << 4) == '\0')
#endif
        {
			return "[ERROR!!!]: malformed percent-encoded sequence";
		}
		tail += 3;
	}

	return Result;
}

Tilc::Apps::Www::TWwwApp::TWwwApp(const Tilc::TExtString& AllowedRootDir, const Tilc::TExtString& AppName, const Tilc::TExtString& AppSlug, const Tilc::TExtString& AllowOrigin)
    : m_AllowedRootDir(AllowedRootDir), m_AppName(AppName), m_AppSlug(AppSlug), m_AllowOrigin(AllowOrigin)
{
}

Tilc::Apps::Www::TWwwApp::~TWwwApp()
{
}

void Tilc::Apps::Www::TWwwApp::AddAvailableLanguages(const std::initializer_list<const char*>& Languages)
{
	std::for_each(Languages.begin(), Languages.end(), [this](Tilc::TExtString lang) {
		m_Languages.push_back(lang);
	});
}
