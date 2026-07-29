#pragma once

#include <vector>

#include "Tilc/DllGlobals.h"
#include "Tilc/Utils/ExtString.h"
#include "Tilc/Net/Http.h"

namespace Tilc
{
    namespace Commerce
    {
        class DECLSPEC TPayU
        {
            public:
                enum EPaymentConfigType {
                    PaymentTypeSandbox,
                    PaymentTypeProduction
                };

                struct TPaymentConfig
                {
                    Tilc::TExtString Url;
                    Tilc::TExtString PosId;
                    Tilc::TExtString Md5Key;
                    Tilc::TExtString ClientId;
                    Tilc::TExtString ClientSecret;
                };

                TPayU();
                ~TPayU();

                void AddConfig(EPaymentConfigType ConfigType, TPaymentConfig Config);
                void SetConfig(EPaymentConfigType ConfigType)
                {
                    m_PaymentType = ConfigType;
                }
                Tilc::TExtString Login(const char* GrantType = "client_credentials");
                Tilc::TExtString GetBearer() const {
                    return m_Bearer;
                }

            protected:
                std::vector<TPaymentConfig> m_Config;
                Tilc::Net::THttp m_Http;
                Tilc::TExtString m_Bearer;
                EPaymentConfigType m_PaymentType{PaymentTypeSandbox};
        };
    }
}
