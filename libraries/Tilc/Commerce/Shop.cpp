#include "Tilc/Commerce/Shop.h"

std::vector<Tilc::Commerce::TProduct*> Tilc::Commerce::TObserver::products;


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
        totalPrice = totalPrice + product->price;
    }
}

