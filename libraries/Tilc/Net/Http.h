#pragma once
#include <vector>
#include <map>
#include <curl/curl.h>

#include "Tilc/DllGlobals.h"
#include "Tilc/Utils/ExtString.h"

namespace Tilc
{
    namespace Net {
        class DECLSPEC THttp
        {
            using TVariableValue = std::pair<Tilc::TExtString, Tilc::TExtString>;
            using TVariableArray = std::pair<Tilc::TExtString, std::vector<Tilc::TExtString>>;
            using TVariables = std::map<Tilc::TExtString, TVariableArray>;

            public:
                THttp();
                ~THttp();

                TVariables m_Variables;
                Tilc::TExtString& DoPost(Tilc::TExtString Url, Tilc::TExtString Variables, Tilc::TExtString& ResultCode);
                Tilc::TExtString& DoPost(Tilc::TExtString Url, Tilc::TExtString& ResultCode);

            protected:
                CURL *m_Curl;
                CURLcode m_InitResult;
                CURLcode m_Result;
                Tilc::TExtString m_ResponseData;

                static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp);
        };
    }
}
