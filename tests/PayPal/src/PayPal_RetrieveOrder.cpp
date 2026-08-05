#include <iostream>
#include "Tilc/Commerce/PayPal.h"
#include "Tilc/Commerce/Shop.h"
#include "Tilc/Utils/ExtString.h"
#include "Tilc/Utils/JsonParser.h"
#include "Tilc/Utils/StdObject.h"


int main(int argc, char* argv[])
{
    Tilc::Commerce::TPayPal p;
    p.SetShopDescription("Tilc Mega Store");
    
    p.AddConfig(Tilc::Commerce::TPayment::PaymentTypeSandbox, {
        "https://api-m.sandbox.paypal.com",
        "", // PosId
        "", // Md5Sum
        "", // OAuth ClientId
        "" // OAuth ClientSecret
    });
    p.AddConfig(Tilc::Commerce::TPayment::PaymentTypeProduction, {
        "https://api-m.paypal.com",
        "", // PosId
        "", // Md5Sum
        "", // OAuth ClientId
        "" // OAuth ClientSecret
    });

    Tilc::TExtString Result, PayPalOrderId = "58P286432E6901128";
    Result = p.Login();
    Tilc::TStdObject* Order = nullptr;
    Result = p.RetrieveOrder(PayPalOrderId, &Order);
    if (Result == "OK")
    {
        std::cout << Order->toJson() << std::endl;
    }
    if (Order)
    {
        delete Order;
        Order = nullptr;
    }
    return 0;
}
