#pragma once

#include <string>
#include <iostream>
#include <thread>
#include <fstream>
#include <algorithm>
#include <vector>
#include <unordered_map>
#include <functional>
#include <iterator>
#include <fcgio.h>
#include <fcgiapp.h>
#ifdef __linux__
    #include <unistd.h>
#endif

#include "Tilc/Apps/Www/WwwApp.h"
#include "Tilc/Utils/Log.h"

namespace Tilc
{
    namespace Apps
    {
        namespace Www
        {
            class DECLSPEC TRequestHandler
            {
            public:
                TRequestHandler(FCGX_Request* Request);
                ~TRequestHandler();
                void HandleRequest();

                void Init();
                void Cleanup();

                void ReadPostData();

                fcgi_streambuf cin_fcgi_streambuf;
                fcgi_streambuf cout_fcgi_streambuf;
                fcgi_streambuf cerr_fcgi_streambuf;
                std::ostream os;
                std::ostream errs;
                std::istream is;

                Tilc::TLog Log;

                Tilc::TExtString QueryString;
                Tilc::TExtString ScriptName;
                Tilc::TExtString RequestUri;
                Tilc::TExtString Referer;
                Tilc::TExtString UserAgent;
                Tilc::TExtString ScriptFileName;
                Tilc::TExtString ApplicationRootDir;
                std::vector<Tilc::TExtString> UriParts;
                ERequestMethod RequestMethod;
                Tilc::TExtString Lang;

                std::vector<Tilc::TExtString> Headers;

                // PostData
                long int ContentLength{ 0 };
                Tilc::TExtString Body;
                Tilc::TExtString Bearer;
                std::unordered_map<Tilc::TExtString, Tilc::TExtString> PostVars;

                TRequestHandler& operator<<(const std::string& val);
                TRequestHandler& operator<<(const char* val);
                TRequestHandler& operator<<(int val);

                // Total number of handled requests
                static long long int Count;

                FCGX_Request* GetRequest() const { return request; };

                std::unordered_map<Tilc::TExtString, std::function<void(const Tilc::TExtString&)>> m_RequestHandlers;

            protected:
                FCGX_Request* request = nullptr;
            };
        }
    }
}
