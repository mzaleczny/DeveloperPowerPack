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

Tilc::TExtString Tilc::Commerce::TPayU::MakeOrder(TCart* cart, TCheckout* checkout, Tilc::TExtString ClientIp,
    Tilc::TExtString ContinueUrl, Tilc::TExtString NotifyUrl, Tilc::TExtString ExtOrderId,
    Tilc::TExtString& RedirectUri, Tilc::TExtString& CreatedOrderId
)
{
    Tilc::TExtString ResultCode;
    Tilc::TExtString JsonData;
    JsonData.reserve(512);
    JsonData += "{";
        JsonData += "\"continueUrl\": \"" + ContinueUrl + "\",";
        JsonData += "\"notifyUrl\": \"" + NotifyUrl + "\",";
        JsonData += "\"extOrderId\": \"" + ExtOrderId + "\",";
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
    //std::cout << o.toJson() << std::endl;
    o.clean();
    
    Tilc::TExtString ResultJson = m_Http.DoPost(m_Config[m_PaymentType].Url + "/api/v2_1/orders", JsonData,
        {
            "Content-type: application/json",
            Tilc::TExtString("Authorization: Bearer ") + m_Bearer
        },
        ResultCode);
    
    if (ResultCode != "OK")
    {
        std::cerr << ResultCode << std::endl;
        ResultJson = "";
    }
    std::cout << ResultJson << std::endl;
    RedirectUri = "";
    CreatedOrderId = "";
    p.parse(ResultJson, &o);
    if (o.getAsObject("root"))
    {
        TStdObject* Result = o.getAsObject("root")->getAsObject("status");
        if (Result)
        {
            Tilc::TExtString StatusCode = Result->getAsString("statusCode");
            if (StatusCode == "SUCCESS")
            {
                RedirectUri = o.getAsObject("root")->getAsString("redirectUri");
                CreatedOrderId = o.getAsObject("root")->getAsString("orderId");
                ResultJson = "OK";
            }
            else if (StatusCode == "UNAUTHORIZED")
            {
                ResultJson = "ERROR:" + Result->getAsString("code") + "-" + Result->getAsString("codeLiteral") + ":" + Result->getAsString("statusDesc");
            }
        }
    }
    return ResultJson;
}
