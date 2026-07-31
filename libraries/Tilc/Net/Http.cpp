#include <iostream>

#include "Tilc/Net/Http.h"


size_t Tilc::Net::THttp::WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    THttp* http = static_cast<THttp*>(userp);
    if (http)
    {
        http->m_ResponseData.append((char*)contents, size * nmemb);
        return size * nmemb;
    }
    return 0;
}

Tilc::Net::THttp::THttp()
{
    m_InitResult = curl_global_init(CURL_GLOBAL_ALL);
}

Tilc::Net::THttp::~THttp()
{
    curl_global_cleanup();
}

Tilc::TExtString& Tilc::Net::THttp::DoPost(Tilc::TExtString Url, Tilc::TExtString Variables, std::vector<Tilc::TExtString> Headers, Tilc::TExtString& ResultCode)
{
    struct curl_slist* headers = nullptr;

    m_ResponseData = "";
    if (m_InitResult != CURLE_OK)
    {
        ResultCode = Tilc::TExtString("curl_easy_perform() failed: ") + curl_easy_strerror(m_InitResult) + "\n";
        return m_ResponseData;
    }

    m_Curl = curl_easy_init();
    if (m_Curl)
    {
        curl_easy_setopt(m_Curl, CURLOPT_URL, Url.c_str());
        curl_easy_setopt(m_Curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(m_Curl, CURLOPT_WRITEDATA, this);

        for (size_t i = 0; i < Headers.size(); ++i)
        {
            headers = curl_slist_append(headers, Headers[i].c_str());
        }
        if (headers)
        {
            curl_easy_setopt(m_Curl, CURLOPT_HTTPHEADER, headers);
        }
        if (Variables.length() > 0)
        {
            curl_easy_setopt(m_Curl, CURLOPT_POSTFIELDS, Variables.c_str());
        }

        m_Result = curl_easy_perform(m_Curl);
        if(m_Result != CURLE_OK)
        {
            ResultCode = Tilc::TExtString("curl_easy_perform() failed: ") + curl_easy_strerror(m_Result) + "\n";
            m_ResponseData = "";
            curl_easy_cleanup(m_Curl);
            return m_ResponseData;
        }
        curl_easy_cleanup(m_Curl);
    }

    ResultCode = "OK";
    return m_ResponseData;
}

Tilc::TExtString& Tilc::Net::THttp::DoPost(Tilc::TExtString Url, std::vector<Tilc::TExtString> Headers, Tilc::TExtString& ResultCode)
{
    return m_ResponseData;
}
