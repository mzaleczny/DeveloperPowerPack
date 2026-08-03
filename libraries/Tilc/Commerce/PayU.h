#pragma once

#include <vector>

#include "Tilc/DllGlobals.h"
#include "Tilc/Utils/ExtString.h"
#include "Tilc/Net/Http.h"

namespace Tilc
{
    class TStdObject;

    namespace Commerce
    {
        class TCart;
        class TCheckout;

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
                Tilc::TExtString MakeOrder(TCart* cart, TCheckout* checkout, Tilc::TExtString ClientIp,
                    Tilc::TExtString ContinueUrl, Tilc::TExtString NotifyUrl, Tilc::TExtString ExtOrderId, // ExtOrderId - id zamówienia w naszym sklepie
                    Tilc::TExtString& RedirectUri, Tilc::TExtString& CreatedPayUOrderId // CreatedPayUOrderId - id zamówienia w PayU
                );
                Tilc::TExtString RetrieveOrder(const Tilc::TExtString& PayUOrderId, Tilc::TStdObject** OrderData);
                void SetShopDescription(Tilc::TExtString Description) {
                    m_ShopDescription = Description;
                }

            protected:
                std::vector<TPaymentConfig> m_Config;
                Tilc::Net::THttp m_Http;
                Tilc::TExtString m_Bearer;
                Tilc::TExtString m_ShopDescription;
                EPaymentConfigType m_PaymentType{PaymentTypeSandbox};
        };
    }
}
