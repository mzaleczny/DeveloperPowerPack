#pragma once

#include "Tilc/DllGlobals.h"
#include "Tilc/Utils/ExtString.h"
#include <vector>

namespace Tilc
{
    namespace Commerce
    {
        class TProduct;

        class DECLSPEC TObserver
        {
        public:
            virtual void update(TProduct* product) = 0;
        protected:
            static std::vector<TProduct*> products;
        };
        std::vector<TProduct*> TObserver::products;

        class DECLSPEC TReview
        {
        public:
            virtual void update(TProduct* product) = 0;
        };


        class DECLSPEC TUserReview : public TReview
        {
        public:
            Tilc::TExtString username;
            double score;

            TUserReview(Tilc::TExtString username, double score)
            {
                this->username = username;
                this->score = score;
            }
            Tilc::TExtString getUsername() const
            {
                return username;
            }
            double getScore() const
            {
                return score;
            }
            void update(TProduct* product);
        };

        class DECLSPEC TProduct
        {
        public:
            Tilc::TExtString name;
            Tilc::TExtString slug;
            Tilc::TExtString short_description;
            double price;
            Tilc::TExtString mini_map_file;
            Tilc::TExtString css_class;
            Tilc::TExtString code;


            // Constructor
            TProduct(Tilc::TExtString name, Tilc::TExtString slug, Tilc::TExtString short_description, double price,
                    Tilc::TExtString mini_map_file, Tilc::TExtString css_class, Tilc::TExtString code)
            {
                this->name = name;
                this->slug = slug;
                this->short_description = short_description;
                this->price = price;
                this->mini_map_file = mini_map_file;
                this->css_class = css_class;
                this->code = code;
            }

            // Observer pattern methods
            void attach(Observer* observer) {
                observers.push_back(observer);
                observer->update(this);
            }
            void detach(Observer* observer) {
                observers.erase(std::remove(observers.begin(), observers.end(), observer), observers.end());
            }
            void notify() {
                for (auto observer : observers) {
                    observer->update(this);
                }
            }

            // Review methods
            void addReview(UserReview* review) {
                reviews.push_back(review);

                // Update the average review score
                double totalScore = averageReviewScore * (reviews.size() - 1) + review->getScore();
                averageReviewScore = totalScore / reviews.size();

                // Notify all reviews of the updated average review score
                notifyReviews();
            }
            void removeReview(UserReview* review) {
                reviews.erase(std::remove(reviews.begin(), reviews.end(), review), reviews.end());

                // Update the average review score
                averageReviewScore = getTotalScore() / reviews.size();
                // Notify all reviews of the updated average review score
                notifyReviews();
            }
            void notifyReviews() {
                for (auto review : reviews) {
                    review->update(this);
                }
            }
            double getTotalScore()
            {
                double total = 0.0;
                for (auto review : reviews) {
                    total += (reinterpret_cast<UserReview*>(review))->getScore();
                }
                return total;
            }
        };

        class DECLSPEC TProductFull : public TProduct
        {
        public:
            Tilc::TExtString name_en;
            Tilc::TExtString description;
            double price_1;
            double price_2;
            double price_3;
            double price_eur;
            double price_dol;
            std::vector<Tilc::TExtString> pictures;
            Tilc::TExtString created;
            Tilc::TExtString modified;
            int inventoryLevel;
            std::vector<Observer*> observers;
            std::vector<Review*> reviews;
            double averageReviewScore;

            // Constructor
            TProductFull(Tilc::TExtString name, Tilc::TExtString slug, Tilc::TExtString short_description, double price,
                Tilc::TExtString mini_map_file, Tilc::TExtString css_class, Tilc::TExtString code,
                Tilc::TExtString name_en, Tilc::TExtString description, double price_1, double price_2, double price_3, double price_eur, double price_dol,
                Tilc::TExtString created, Tilc::TExtString modified, int inventoryLevel
            ) : TProduct(name, slug, short_description, price, mini_map_file, css_class, code)
            {
                this->name_en = name_en;
                this->description = description;
                this->price_1 = price_1;
                this->price_2 = price_2;
                this->price_3 = price_3;
                this->price_eur = price_eur;
                this->price_dol = price_dol;
                this->created = created;
                this->modified = modified;
                this->inventoryLevel = inventoryLevel;
                this->averageReviewScore = 0.0;
            }
        };

        class DECLSPEC Cart : public TObserver
        {
        public:
            void addProduct(TProduct* product)
            {
                products.push_back(product);
                product->attach(this);
            }
            void update(TProduct* product)
            {
                // Check if the product is in the cart and remove it if inventory level reaches zero
                for (auto it = products.begin(); it != products.end(); it++)
                {
                    if ((*it) == product && product->getInventoryLevel() == 0)
                    {
                        products.erase(it);
                        break;
                    }
                }
            }
            size_t size()
            {
                return products.size();
            }
        };

        class TCheckout : public TObserver
        {
        private:
            double totalPrice;
        public:
            void update(TProduct* product)
            {
                // Recalculate total price when inventory level changes
                totalPrice = 0;
                // Loop through all products in the cart and recalculate the total price
                // This assumes that the cart is already populated with products
                for (auto product : products)
                {
                    totalPrice += product->getPrice();
                }
            }
            void addProduct(TProduct* product)
            {
                products.push_back(product);
                product->attach(this);
                update(product);
            }
            double getTotalPrice()
            {
                return totalPrice;
            }
        };
    }
}
