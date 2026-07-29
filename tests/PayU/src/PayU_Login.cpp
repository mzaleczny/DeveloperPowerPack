#include <iostream>
#include "Tilc/Commerce/PayU.h"
#include "Tilc/Utils/ExtString.h"
#include "Tilc/Utils/JsonParser.h"
#include "Tilc/Utils/StdObject.h"

int main(int argc, char* argv[])
{
    Tilc::Commerce::TPayU p;
    p.AddConfig(Tilc::Commerce::TPayU::PaymentTypeSandbox, {
        "https://merch-prod.snd.payu.com",
        "", // PosId
        "", // Md5Sum
        "", // OAuth ClientId
        "" // OAuth ClientSecret
    });
    p.AddConfig(Tilc::Commerce::TPayU::PaymentTypeProduction, {
        "https://secure.payu.com",
        "", // PosId
        "", // Md5Sum
        "", // OAuth ClientId
        "" // OAuth ClientSecret
    });

    Tilc::TExtString JsonString = p.Login();
    std::cout << "Bearer: " << p.GetBearer() << std::endl << std::endl;

    return 0;
}
