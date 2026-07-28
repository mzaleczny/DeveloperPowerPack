#include "Tilc/Commerce/PayU.h"
#include <iostream>

Tilc::Commerce::TPayU::TPayU()
{
    m_Config.resize(2);
}

Tilc::Commerce::TPayU::~TPayU()
{
}

void Tilc::Commerce::TPayU::AddConfig(EPaymentConfigType ConfigType, TPaymentConfig Config)
{
    m_Config[ConfigType] = Config;
}

Tilc::TExtString Tilc::Commerce::TPayU::Login(const char* GrantType)
{
    Tilc::TExtString ResultCode;
    Tilc::TExtString Variables;
    Variables = Tilc::TExtString("grant_type=") + GrantType;
    Variables += Tilc::TExtString("&client_id=") + m_Config[m_PaymentType].ClientId;
    Variables += Tilc::TExtString("&client_secret=") + m_Config[m_PaymentType].ClientSecret;
    Tilc::TExtString Result = m_Http.DoPost(m_Config[m_PaymentType].Url + "/pl/standard/user/oauth/authorize", Variables, ResultCode);
    if (ResultCode != "OK")
    {
        std::cerr << ResultCode << std::endl;
        Result = "";
    }
    return Result;
}
