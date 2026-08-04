#pragma once

#include <vector>

#include "Tilc/DllGlobals.h"
#include "Tilc/Utils/ExtString.h"
#include "Tilc/Net/Http.h"
#include "Tilc/Commerce/Payment.h"

namespace Tilc
{
    class TStdObject;

    namespace Commerce
    {
        class DECLSPEC TPayU : public TPayment
        {
            public:
                TPayU();
                ~TPayU();

                virtual Tilc::TExtString Login(const char* GrantType = "client_credentials") override;
                virtual Tilc::TExtString MakeOrder(TCart* cart, TCheckout* checkout, Tilc::TExtString ClientIp,
                    Tilc::TExtString ContinueUrl, Tilc::TExtString NotifyUrl, Tilc::TExtString ExtOrderId, // ExtOrderId - id zamówienia w naszym sklepie
                    Tilc::TExtString& RedirectUri, Tilc::TExtString& CreatedOrderId // CreatedPayUOrderId - id zamówienia w PayU
                ) override;
                virtual Tilc::TExtString RetrieveOrder(const Tilc::TExtString& PayUOrderId, Tilc::TStdObject** OrderData) override;
        };
    }
}
