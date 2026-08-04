#include <iostream>
#include "Tilc/Commerce/PayU.h"
#include "Tilc/Commerce/Shop.h"
#include "Tilc/Utils/ExtString.h"
#include "Tilc/Utils/JsonParser.h"
#include "Tilc/Utils/StdObject.h"


int main(int argc, char* argv[])
{
    Tilc::Commerce::TPayU p;
    p.SetShopDescription("Tilc Mega Store");
    
    p.AddConfig(Tilc::Commerce::TPayment::PaymentTypeSandbox, {
        "https://merch-prod.snd.payu.com",
        "", // PosId
        "", // Md5Sum
        "", // OAuth ClientId
        "" // OAuth ClientSecret
    });
    p.AddConfig(Tilc::Commerce::TPayment::PaymentTypeProduction, {
        "https://secure.payu.com",
        "", // PosId
        "", // Md5Sum
        "", // OAuth ClientId
        "" // OAuth ClientSecret
    });

    Tilc::TExtString Result, PayUOrderId = "H5BZFR854L260803GUEST000P01";
    Result = p.Login();
    Tilc::TStdObject* Order = nullptr;
    Result = p.RetrieveOrder(PayUOrderId, &Order);
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
