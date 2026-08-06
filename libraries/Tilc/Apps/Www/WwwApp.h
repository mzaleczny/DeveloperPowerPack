#pragma once

#include <vector>
#include "Tilc/Utils/ExtString.h"

namespace Tilc
{
    namespace Apps
    {
        namespace Www
        {

            enum class ERequestMethod
            {
                ERM_GET,
                ERM_HEAD,
                ERM_POST,
                ERM_PUT,
                ERM_DELETE,
                ERM_INVALID
            };

            struct DECLSPEC TVariable
            {
                Tilc::TExtString Name;
                Tilc::TExtString Value;
            };

            extern long long int Count;
            extern std::string DbHost;
            extern std::string DbName;
            extern std::string DbUser;
            extern std::string DbPasswd;
            extern class TRequestHandler* rh;
            extern class TWwwApp* Application;

            ERequestMethod ToRequestMethod(std::string method);
            std::string urlencode(const std::string& Input);
            std::string urldecode(const std::string& Input);

            class DECLSPEC TWwwApp
            {
            public:
                // AllowOrigin origin should be full url, ex: https://www.yourdomain.pl
                TWwwApp(const Tilc::TExtString& AllowedRootDir, const Tilc::TExtString& AppName, const Tilc::TExtString& AppSlug, const Tilc::TExtString& AllowOrigin);
                ~TWwwApp();

                const Tilc::TExtString& GetAllowedRootDir() const { return m_AllowedRootDir; }
                const Tilc::TExtString& GetAppName() const { return m_AppName; }
                const Tilc::TExtString& GetAppSlug() const { return m_AppSlug; }
                const std::vector<Tilc::TExtString>& GetLanguages() const { return m_Languages; }
                const Tilc::TExtString& GetAllowOrigin() const { return m_AllowOrigin; }

            protected:
                Tilc::TExtString m_AllowedRootDir;
                Tilc::TExtString m_AllowOrigin;
                Tilc::TExtString m_AppName;
                Tilc::TExtString m_AppSlug;
                std::vector<Tilc::TExtString> m_Languages;
            };
        }
    }
}
