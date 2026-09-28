#include "Tilc/Commerce/Shop.h"
#include "Tilc/Utils/JsonParser.h"
#include <iostream>

#include "Tilc/Utils/StdObject.h"

std::vector<Tilc::Commerce::TProduct*> Tilc::Commerce::TObserver::products;

Tilc::TStdObject* Tilc::Commerce::TDataObject::GetDataObject(const Tilc::TExtString& JsonContent)
{
    Tilc::TJsonParser JsonParser;
    Tilc::TStdObject* JsonRoot = JsonParser.parse(JsonContent);
    Tilc::TStdObject* JsonObject{};
    if (JsonRoot)
    {
        JsonObject = JsonRoot->getAsObject("root");
        if (!JsonObject)
        {
            JsonObject = JsonRoot;
        }
        if (JsonObject)
        {
            Tilc::TPropertiesVector* Items = JsonObject->getAsArray("items");
            if (Items && Items->size() == 1)
            {
                JsonObject = (*Items)[0]->oValue;
            }
        }
        if (JsonObject)
        {
            JsonObject = JsonObject->clone();
        }
        delete JsonRoot;
        return JsonObject;
    }
    return nullptr;
}
Tilc::TExtString Tilc::Commerce::TCategory::ToJson()
{
    Tilc::TExtString n = EscapeString(name);
    Tilc::TExtString sd = EscapeString(short_description);
    Tilc::TExtString d = EscapeString(description);
    return Tilc::TExtString("{\n") +
            "\"id\": \"" + id + "\",\n" +
            "\"name\": \"" + n + "\",\n" +
            "\"slug\": \"" + slug + "\",\n" +
            "\"short_description\": \"" + sd + "\",\n" +
            "\"description\": \"" + d + "\"\n" +
        "}";
}

void Tilc::Commerce::TCategory::FromJson(const Tilc::TExtString& JsonContent)
{
    Tilc::TStdObject* JsonObject = GetDataObject(JsonContent);
    if (JsonObject)
    {
        FromJsonObject(JsonObject);
        delete JsonObject;
    }
}

void Tilc::Commerce::TCategory::FromJsonObject(Tilc::TStdObject* JsonObject)
{
    if (JsonObject)
    {
        id = JsonObject->getAsString("id");
        name = JsonObject->getAsStringUnescaped("name");
        slug = JsonObject->getAsString("slug");
        short_description = JsonObject->getAsStringUnescaped("short_description");
        description = JsonObject->getAsStringUnescaped("description");
    }
}

bool Tilc::Commerce::TCategory::IsEmpty() const
{
    return id.empty() && name.empty() && short_description.empty() && description.empty();
}

std::unordered_map<Tilc::TExtString, Tilc::TExtString> Tilc::Commerce::TCategory::GetDataMap() const
{
    std::unordered_map<Tilc::TExtString, Tilc::TExtString> Data;
    Data["txtId"] = id;
    Data["txtName"] = name;
    Data["txtSlug"] = slug;
    Data["txtShortDescription"] = short_description;
    Data["txtDescription"] = description;
    return Data;
}

void Tilc::Commerce::TCategory::SetDataFromMap(std::unordered_map<Tilc::TExtString, Tilc::TExtString> Data)
{
    name = Data["txtName"];
    slug = Tilc::ToSlug(name);
    short_description = Data["txtShortDescription"];
    description = Data["txtDescription"];
}

std::vector<Tilc::TExtString> Tilc::Commerce::TCategory::GetDataForListColumns() const
{
    return {name, slug, short_description};
}


Tilc::TExtString Tilc::Commerce::TProduct::ToJson()
{
    return Tilc::TExtString("{\n") +
            "\"id\": \"" + id + "\",\n" +
            "\"name\": \"" + EscapeString(name) + "\",\n" +
            "\"slug\": \"" + slug + "\",\n" +
            "\"short_description\": \"" + EscapeString(short_description) + "\",\n" +
            "\"description\": \"" + EscapeString(description) + "\",\n" +
            "\"price\": \"" + price.ToString('.') + "\",\n" +
            "\"price_1\": \"" + price_1.ToString('.') + "\",\n" +
            "\"price_2\": \"" + price_2.ToString('.') + "\",\n" +
            "\"price_3\": \"" + price_3.ToString('.') + "\",\n" +
            "\"mini_map_file\": \"" + EscapeString(mini_map_file) + "\",\n" +
            "\"css_class\": \"" + EscapeString(css_class) + "\",\n" +
            "\"product_code\": \"" + EscapeString(product_code) + "\"\n" +
        "}";
}

void Tilc::Commerce::TProduct::FromJson(const Tilc::TExtString& JsonContent)
{
    Tilc::TStdObject* JsonObject = GetDataObject(JsonContent);
    if (JsonObject)
    {
        FromJsonObject(JsonObject);
        delete JsonObject;
    }
}

void Tilc::Commerce::TProduct::FromJsonObject(Tilc::TStdObject* JsonObject)
{
    if (JsonObject)
    {
        id = JsonObject->getAsString("id");
        name = JsonObject->getAsStringUnescaped("name");
        slug = JsonObject->getAsString("slug");
        short_description = JsonObject->getAsStringUnescaped("short_description");
        description = JsonObject->getAsStringUnescaped("description");
        price.FromString(JsonObject->getAsStringUnescaped("price"), '.');
        price_1.FromString(JsonObject->getAsStringUnescaped("price_1"), '.');
        price_2.FromString(JsonObject->getAsStringUnescaped("price_2"), '.');
        price_3.FromString(JsonObject->getAsStringUnescaped("price_3"), '.');
        mini_map_file = JsonObject->getAsStringUnescaped("mini_map_file");
        css_class = JsonObject->getAsStringUnescaped("css_class");
        product_code = JsonObject->getAsStringUnescaped("product_code");
    }
}

bool Tilc::Commerce::TProduct::IsEmpty() const
{
    return id.empty() && name.empty() && slug.empty() && short_description.empty() &&
        price.GetTotalAmountInt() == 0 && price_1.GetTotalAmountInt() == 0 && price_2.GetTotalAmountInt() == 0 && price_3.GetTotalAmountInt() == 0 &&
        mini_map_file.empty() && css_class.empty() && product_code.empty();
}

std::unordered_map<Tilc::TExtString, Tilc::TExtString> Tilc::Commerce::TProduct::GetDataMap() const
{
    std::unordered_map<Tilc::TExtString, Tilc::TExtString> Data;
    Data["txtId"] = id;
    Data["txtName"] = name;
    Data["txtSlug"] = slug;
    Data["txtShortDescription"] = short_description;
    Data["txtDescription"] = description;
    Data["txtPrice"] = price.ToString();
    Data["txtPrice1"] = price_1.ToString();
    Data["txtPrice2"] = price_2.ToString();
    Data["txtPrice3"] = price_3.ToString();
    Data["txtMiniMapFile"] = mini_map_file;
    Data["txtCssClass"] = css_class;
    Data["txtProductCode"] = product_code;
    return Data;
}

void Tilc::Commerce::TProduct::SetDataFromMap(std::unordered_map<Tilc::TExtString, Tilc::TExtString> Data)
{
    name = Data["txtName"];
    slug = Tilc::ToSlug(name);
    short_description = Data["txtShortDescription"];
    description = Data["txtDescription"];
    price = Data["txtPrice"];
    price_1= Data["txtPrice1"];
    price_2= Data["txtPrice2"];
    price_3 = Data["txtPrice3"];
    mini_map_file = Data["txtMiniMapFile"];
    css_class = Data["txtCssClass"];
    product_code = Data["txtProductCode"];
}

std::vector<Tilc::TExtString> Tilc::Commerce::TProduct::GetDataForListColumns() const
{
    return {name, slug, product_code, price.ToString(), short_description};
}

void Tilc::Commerce::TProduct::addReview(TUserReview* review)
{
    reviews.push_back(review);

    // Update the average review score
    double totalScore = averageReviewScore * (reviews.size() - 1) + review->getScore();
    averageReviewScore = totalScore / reviews.size();

    // Notify all reviews of the updated average review score
    notifyReviews();
}


void Tilc::Commerce::TProduct::removeReview(TUserReview* review)
{
    reviews.erase(std::remove(reviews.begin(), reviews.end(), review), reviews.end());

    // Update the average review score
    averageReviewScore = getTotalScore() / reviews.size();
    // Notify all reviews of the updated average review score
    notifyReviews();
}

double Tilc::Commerce::TProduct::getTotalScore()
{
    double total = 0.0;
    for (auto review : reviews)
    {
        total += (reinterpret_cast<TUserReview*>(review))->getScore();
    }
    return total;
}




void Tilc::Commerce::TCart::update(TProduct* product)
{
    // Check if the product is in the cart and remove it if inventory level reaches zero
    for (auto it = products.begin(); it != products.end(); it++)
    {
        if ((*it) == product && product->inventoryLevel == 0)
        {
            products.erase(it);
            break;
        }
    }
}


void Tilc::Commerce::TCheckout::update(TProduct* product)
{
    // Recalculate total price when inventory level changes
    totalPrice = 0;
    // Loop through all products in the cart and recalculate the total price
    // This assumes that the cart is already populated with products
    for (auto product : products)
    {
        totalPrice = totalPrice + product->price * product->inventoryLevel;
    }
}

