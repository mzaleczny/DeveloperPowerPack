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
        id = JsonObject->getAsString("id");
        name = JsonObject->getAsStringUnescaped("name");
        slug = JsonObject->getAsString("slug");
        short_description = JsonObject->getAsStringUnescaped("short_description");
        description = JsonObject->getAsStringUnescaped("description");
        delete JsonObject;
    }
}

bool Tilc::Commerce::TCategory::IsEmpty()
{
    return id.empty() && name.empty() && short_description.empty() && description.empty();
}

std::unordered_map<Tilc::TExtString, Tilc::TExtString> Tilc::Commerce::TCategory::GetDataMap()
{
    std::unordered_map<Tilc::TExtString, Tilc::TExtString> Data;
    Data["txtId"] = id;
    Data["txtName"] = name;
    Data["txtSlug"] = slug;
    Data["txtShortDescription"] = short_description;
    Data["txtDescription"] = description;
    return Data;
}

void Tilc::Commerce::TCategory::SetDataFromMap(std::unordered_map<Tilc::TExtString, Tilc::TExtString>& Data)
{
    name = Data["txtName"];
    slug = Tilc::ToSlug(name);
    short_description = Data["txtShortDescription"];
    description = Data["txtDescription"];
}

std::vector<Tilc::TExtString> Tilc::Commerce::TCategory::GetDataForListColumns()
{
    return {name, slug, short_description};
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

