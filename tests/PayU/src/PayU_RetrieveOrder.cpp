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
    
    p.AddConfig(Tilc::Commerce::TPayU::PaymentTypeSandbox, {
        "https://merch-prod.snd.payu.com",
        "475138", // PosId
        "a427cabc4ae80f928b7fef9e24778459", // Md5Sum
        "475138", // OAuth ClientId
        "7f5b563529b8e9dd9dff796d4fdcdbb0" // OAuth ClientSecret
        });
    p.AddConfig(Tilc::Commerce::TPayU::PaymentTypeProduction, {
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
