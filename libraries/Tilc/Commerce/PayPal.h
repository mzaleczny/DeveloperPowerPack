#pragma once

#include <vector>

#include "Tilc/DllGlobals.h"
#include "Tilc/Globals.h"
#include "Tilc/Utils/ExtString.h"
#include "Tilc/Net/Http.h"
#include "Tilc/Commerce/Payment.h"
#include "Tilc/Utils/StdObject.h"

namespace Tilc
{
    class TStdObject;

    namespace Commerce
    {
        class DECLSPEC TPayPal : public TPayment
        {
            public:
                TPayPal();
                ~TPayPal();

                virtual Tilc::TExtString Login(const char* GrantType = "client_credentials") override;
                // Dla PayPala ważny jest tylko ContinueUrl. NotifyUrl można zostawić puste.
                // Dla zamówienia nie opłaconego zostanie wywołany ContinueUrl tylko ze zmienną token, np:
                //    https://shop.pl/shop/pl/order-placed?token=12H07867RN172351E
                // Dla zamówienia opłaconego zostanie wywołany ContinueUrl ze zmiennymi token oraz PayerID, np:
                //    https://shop.pl/shop/pl/order-placed?token=12H07867RN172351E&PayerID=62FXPNBQNVZBN
                virtual Tilc::TExtString MakeOrder(TCart* cart, TCheckout* checkout, Tilc::TExtString ClientIp,
                    Tilc::TExtString ContinueUrl, Tilc::TExtString NotifyUrl, Tilc::TExtString ExtOrderId, // ExtOrderId - id zamówienia w naszym sklepie
                    Tilc::TExtString& RedirectUri, Tilc::TExtString& CreatedOrderId // CreatedPayUOrderId - id zamówienia w PayU
                ) override;
                virtual Tilc::TExtString RetrieveOrder(const Tilc::TExtString& PayUOrderId, Tilc::TStdObject** OrderData) override;
                Tilc::TExtString GetRedirectUriForOrder(Tilc::TPropertiesVector* Links);
        };
    }
}
