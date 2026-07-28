#include <iostream>
#include "Tilc/Commerce/PayU.h"
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
    std::cout << "Bearer: " << JsonString << std::endl << std::endl;
    if (JsonString.find("access_token") != std::string::npos)
    {
        Tilc::TJsonParser Parser;
        Tilc::TStdObject Json;
        Parser.parse(JsonString, &Json);
        std::cout << Json.toJson() << std::endl;
    }
    return 0;
}
