#include "Tilc/Commerce/PayU.h"
#include "Tilc/Commerce/Shop.h"
#include "Tilc/Utils/JsonParser.h"
#include "Tilc/Utils/StdObject.h"
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
    Tilc::TExtString ResultJson = m_Http.DoPost(m_Config[m_PaymentType].Url + "/pl/standard/user/oauth/authorize", Variables, {}, ResultCode);
    if (ResultCode != "OK")
    {
        std::cerr << ResultCode << std::endl;
        ResultJson = "";
    }

    if (ResultJson.find("access_token") != std::string::npos)
    {
        Tilc::TJsonParser Parser;
        Tilc::TStdObject Json;
        Parser.parse(ResultJson, &Json);
        m_Bearer = Json.getAsObject("root")->getAsString("access_token");
    }

    return ResultJson;
}

Tilc::TExtString Tilc::Commerce::TPayU::MakeOrder(TCart* cart, TCheckout* checkout, Tilc::TExtString ClientIp)
{
    Tilc::TExtString ResultCode;
    Tilc::TExtString JsonData;
    JsonData.reserve(512);
    JsonData += "{";
        JsonData += "\"customerIp\": \"" + ClientIp + "\",";
        JsonData += "\"merchantPosId\": \"" + m_Config[m_PaymentType].PosId + "\",";
        JsonData += "\"description\": \"" + m_ShopDescription + "\",";
        JsonData += "\"currencyCode\": \"PLN\",";
        JsonData += "\"totalAmount\" : \"" + std::to_string(checkout->getTotalPrice().GetTotalAmountInt()) + "\",";
        JsonData += "\"products\" : [";
            for (size_t i = 0; i < cart->size(); ++i)
            {
                JsonData += "{";
                JsonData += "\"name\": \"" + cart->products[i]->name + "\",";
                JsonData += "\"unitPrice\": \"" + std::to_string(cart->products[i]->price.GetTotalAmountInt()) + "\",";
                JsonData += "\"quantity\" : \"" + std::to_string(cart->products[i]->inventoryLevel) + "\"";
                JsonData += "}";
                if (i < cart->size() - 1)
                {
                    JsonData += ",";
                }
            }
        JsonData += "],";
        JsonData += "\"payMethods\": {";
            JsonData += "\"payMethod\": {";
            JsonData += "\"type\": \"PBL\",";
            JsonData += "\"value\" : \"c\"";
            JsonData += "}";
        JsonData += "}";
    JsonData += "}";
    TJsonParser p;
    TStdObject o;
    p.parse(JsonData, &o);
    std::cout << o.toJson() << std::endl;

    Tilc::TExtString ResultJson = m_Http.DoPost(m_Config[m_PaymentType].Url + "/api/v2_1/orders", JsonData,
        {
            "Content-type: application/json\r\n",
            Tilc::TExtString("Authorization: Bearer ") + m_Bearer + "\r\n"
        },
        ResultCode);
    if (ResultCode != "OK")
    {
        std::cerr << ResultCode << std::endl;
        ResultJson = "";
    }
    return ResultJson;
}
