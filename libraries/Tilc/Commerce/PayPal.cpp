#include "Tilc/Commerce/PayPal.h"
#include "Tilc/Commerce/Shop.h"
#include "Tilc/Utils/JsonParser.h"
#include "Tilc/Utils/StdObject.h"
#include "Tilc/Utils/Base64.h"
#include <iostream>

Tilc::Commerce::TPayPal::TPayPal()
{
}

Tilc::Commerce::TPayPal::~TPayPal()
{
}

Tilc::TExtString Tilc::Commerce::TPayPal::Login(const char* GrantType)
{
    Tilc::TExtString ResultCode;
    Tilc::TExtString Variables;
    Variables = Tilc::TExtString("grant_type=") + GrantType;
    Tilc::TExtString Credentials = m_Config[m_PaymentType].ClientId + ":" + m_Config[m_PaymentType].ClientSecret;
    Tilc::TExtString CredentialsBase64;
    Tilc::TBase64::Encode(reinterpret_cast<const unsigned char*>(Credentials.c_str()), Credentials.length(), CredentialsBase64);
    Tilc::TExtString ResultJson = m_Http.DoPost(m_Config[m_PaymentType].Url + "/v1/oauth2/token", Variables, {
        "Authorization: Basic <" + CredentialsBase64 + ">",
        "Content-Type: application/x-www-form-urlencoded"
        }, ResultCode);
    //std::cout << ResultJson << std::endl;
    if (ResultCode != "OK")
    {
        std::cerr << ResultCode << std::endl;
        return "";
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

Tilc::TExtString Tilc::Commerce::TPayPal::MakeOrder(TCart* cart, TCheckout* checkout, Tilc::TExtString ClientIp,
    Tilc::TExtString ContinueUrl, Tilc::TExtString NotifyUrl, Tilc::TExtString ExtOrderId,
    Tilc::TExtString& RedirectUri, Tilc::TExtString& CreatedOrderId
)
{
    Tilc::TExtString ResultCode;
    Tilc::TExtString JsonData;
    JsonData.reserve(512);
    JsonData += "{";
        JsonData += "\"intent\": \"CAPTURE\",";
        JsonData += "\"payment_source\": {";
            JsonData += "\"paypal\": {";
                JsonData += "\"experience_context\": {";
                    JsonData += "\"payment_method_preference\": \"IMMEDIATE_PAYMENT_REQUIRED\",";
                    JsonData += "\"landing_page\": \"LOGIN\",";
                    JsonData += "\"shipping_preference\": \"GET_FROM_FILE\",";
                    JsonData += "\"user_action\": \"PAY_NOW\",";
                    JsonData += "\"return_url\": \"" + ContinueUrl + "\",";
                    JsonData += "\"cancel_url\": \"" + ContinueUrl + "\"";
                JsonData += "}\n";
            JsonData += "}\n";
        JsonData += "},\n";

        JsonData += "\"purchase_units\": [";
            JsonData += "{";
                JsonData += "\"invoice_id\": \"" + ExtOrderId + "\",";
                JsonData += "\"amount\": {";
                    JsonData += "\"currency_code\": \"PLN\",";
                    JsonData += "\"value\": \"" + checkout->getTotalPrice().ToString('.') + "\",";
                    JsonData += "\"breakdown\": {";
                        JsonData += "\"item_total\": {";
                            JsonData += "\"currency_code\": \"PLN\",";
                            JsonData += "\"value\": \"" + checkout->getTotalPrice().ToString('.') + "\"";
                        JsonData += "},";
                        JsonData += "\"shipping\": {";
                            JsonData += "\"currency_code\": \"PLN\",";
                            JsonData += "\"value\": \"0.00\"";
                        JsonData += "}";
                    JsonData += "}";
                JsonData += "},";
                JsonData += "\"items\": [";
                for (size_t i = 0; i < cart->size(); ++i)
                {
                    JsonData += "{";
                        JsonData += "\"name\": \"" + cart->products[i]->name + "\",";
                        JsonData += "\"unit_amount\": {";
                            JsonData += "\"currency_code\": \"PLN\",";
                            JsonData += "\"value\": \"" + cart->products[i]->price.ToString('.') + "\"";
                        JsonData += "},";
                        JsonData += "\"quantity\" : \"" + std::to_string(cart->products[i]->inventoryLevel) + "\"";
                    JsonData += "}";
                    if (i < cart->size() - 1)
                    {
                        JsonData += ",";
                    }
                }
                JsonData += "]\n";
            JsonData += "}\n";
        JsonData += "]\n";
    JsonData += "}";
    TJsonParser p;
    TStdObject o;
    p.parse(JsonData, &o);
    //std::cout << o.toJson() << std::endl;
    o.clean();
    
    RedirectUri = "";
    CreatedOrderId = "";

    Tilc::TExtString ResultJson = m_Http.DoPost(m_Config[m_PaymentType].Url + "/v2/checkout/orders", JsonData,
        {
            "Content-type: application/json",
            Tilc::TExtString("Authorization: Bearer ") + m_Bearer
        },
        ResultCode);
    std::cout << ResultJson << std::endl;

    if (ResultCode != "OK")
    {
        std::cerr << ResultCode << std::endl;
        ResultJson = "";
        return ResultJson;
    }

    if (ResultJson.find("AUTHENTICATION_FAILURE") != std::string::npos)
    {
        return "ERROR: Authentication failure";
    }

    p.parse(ResultJson, &o);
    if (o.getAsObject("root"))
    {
        Tilc::TExtString Value = o.getAsObject("root")->getAsString("name");
        if (!Value.empty())
        {
            Value = "ERROR:" + Value + ": " + o.getAsObject("root")->getAsString("message");
            if (o.getAsObject("root")->getAsArray("details"))
            {
                Tilc::TStdObjectProperty* Details = reinterpret_cast<Tilc::TStdObjectProperty*>((*o.getAsObject("root")->getAsArray("details"))[0]);
                Value += ": DETAILS: " + Details->oValue->getAsString("description");
                return Value;
            }
        }
        if (o.getAsObject("root")->getAsString("status") == "PAYER_ACTION_REQUIRED")
        {
            Tilc::TPointersVector* Links = o.getAsObject("root")->getAsArray("links");
            for (size_t i = 0; i < Links->size(); ++i)
            {
                Tilc::TStdObjectProperty* Link = reinterpret_cast<Tilc::TStdObjectProperty*>((*Links)[i]);
                if (Link && Link->oValue->getAsString("rel") == "payer-action")
                {
                    RedirectUri = Link->oValue->getAsString("href");
                }
            }
        }

        Value = o.getAsObject("root")->getAsString("id");
        if (!RedirectUri.empty() && !Value.empty())
        {
            CreatedOrderId = Value;
        }
    }
    return ResultJson;
}

Tilc::TExtString Tilc::Commerce::TPayPal::RetrieveOrder(const Tilc::TExtString& PayPalOrderId, Tilc::TStdObject** OrderData)
{
    TJsonParser p;
    TStdObject o;
    Tilc::TExtString ResultCode;
    Tilc::TExtString ResultJson = m_Http.DoPost(m_Config[m_PaymentType].Url + "/v2/checkout/orders/" + PayPalOrderId, "",
        {
            "Content-type: application/json",
            Tilc::TExtString("Authorization: Bearer ") + m_Bearer
        },
        ResultCode);

    if (ResultCode != "OK")
    {
        std::cerr << ResultCode << std::endl;
        ResultJson = "";
        *OrderData = nullptr;
        return ResultJson;
    }

    //std::cout << ResultJson << std::endl;
    /*
    p.parse(ResultJson, &o);
    if (o.getAsObject("root"))
    {
        TStdObject* Result = o.getAsObject("root")->getAsObject("status");
        if (Result)
        {
            Tilc::TExtString StatusCode = Result->getAsString("statusCode");
            if (StatusCode == "UNAUTHORIZED")
            {
                ResultJson = "ERROR:" + Result->getAsString("code") + "-" + Result->getAsString("codeLiteral") + ":" + Result->getAsString("statusDesc");
                *OrderData = nullptr;
                return ResultJson;
            }
        }
        if (o.getAsObject("root")->getAsArray("orders") && o.getAsObject("root")->getAsArray("orders")->size() == 1)
        {
            *OrderData = nullptr;
            TStdObjectProperty* FirstArrayValue = static_cast<TStdObjectProperty*>((*o.getAsObject("root")->getAsArray("orders"))[0]);
            if (FirstArrayValue)
            {
                *OrderData = FirstArrayValue->oValue->cloneIntoCleanRoot();
                return "OK";
            }
        }
    }
    */
    return ResultJson;
}
