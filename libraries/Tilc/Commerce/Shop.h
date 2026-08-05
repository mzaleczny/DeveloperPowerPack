#pragma once

#include "Tilc/DllGlobals.h"
#include "Tilc/Utils/ExtString.h"
#include "Tilc/Commerce/Money.h"
#include <vector>
#include <algorithm>

namespace Tilc
{
    namespace Commerce
    {
        class TProduct;

        class DECLSPEC TObserver
        {
        public:
            virtual void update(TProduct* product) = 0;
            static std::vector<TProduct*> products;
        };

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
                : username(username), score(score)
            {
            }
            Tilc::TExtString getUsername() const
            {
                return username;
            }
            double getScore() const
            {
                return score;
            }
            void update(TProduct* product) {};
        };

        class DECLSPEC TProduct
        {
        public:
            Tilc::TExtString name;
            Tilc::TExtString slug;
            Tilc::TExtString short_description;
            TMoney price;
            TMoney price1;
            TMoney price2;
            TMoney price3;
            Tilc::TExtString mini_map_file;
            Tilc::TExtString css_class;
            Tilc::TExtString code;
            Tilc::TExtString created;
            Tilc::TExtString modified;
            int inventoryLevel;
            std::vector<TObserver*> observers;
            std::vector<TReview*> reviews;
            double averageReviewScore;

            // Constructor
            TProduct(Tilc::TExtString name, Tilc::TExtString slug, Tilc::TExtString short_description, int price, int inventoryLevel,
                    Tilc::TExtString mini_map_file = "", Tilc::TExtString css_class = "", Tilc::TExtString code = "",
                    Tilc::TExtString created = "", Tilc::TExtString modified = "")
                : name(name), slug(slug), short_description(short_description), price(price), mini_map_file(mini_map_file), css_class(css_class), code(code),
                  created(created), modified(modified), inventoryLevel(inventoryLevel), averageReviewScore(0.0)
            {
            }

            TProduct(Tilc::TExtString name, Tilc::TExtString slug, Tilc::TExtString short_description, double price, int inventoryLevel,
                    Tilc::TExtString mini_map_file = "", Tilc::TExtString css_class = "", Tilc::TExtString code = "",
                    Tilc::TExtString created = "", Tilc::TExtString modified = "")
                : name(name), slug(slug), short_description(short_description), price(price), mini_map_file(mini_map_file), css_class(css_class), code(code),
                  created(created), modified(modified), inventoryLevel(inventoryLevel), averageReviewScore(0.0)
            {
            }

            // Observer pattern methods
            void attach(TObserver* observer)
            {
                observers.push_back(observer);
                observer->update(this);
            }

            void detach(TObserver* observer)
            {
                observers.erase(std::remove(observers.begin(), observers.end(), observer), observers.end());
            }

            void notify()
            {
                for (auto observer : observers)
                {
                    observer->update(this);
                }
            }

            // Review methods
            void addReview(TUserReview* review);
            void removeReview(TUserReview* review);

            inline void notifyReviews()
            {
                for (auto review : reviews)
                {
                    review->update(this);
                }
            }

            double getTotalScore();

            inline void setInventoryLevel(int inventoryLevel)
            {
                this->inventoryLevel = inventoryLevel;
                notify();
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
            std::vector<Tilc::TExtString> pictures;

            // Constructor
            TProductFull(Tilc::TExtString name, Tilc::TExtString slug, Tilc::TExtString short_description, double price, int inventoryLevel,
                Tilc::TExtString mini_map_file = "", Tilc::TExtString css_class = "", Tilc::TExtString code = "",
                Tilc::TExtString created = "", Tilc::TExtString modified = "",
                Tilc::TExtString name_en = "", Tilc::TExtString description = "", int price_1 = 0, int price_2 = 0, int price_3 = 0
                
            ) : TProduct(name, slug, short_description, price, inventoryLevel, mini_map_file, css_class, code, created, modified),
                name_en(name_en), description(description), price_1(price_1), price_2(price_2), price_3(price_3)
            {
            }
        };

        class DECLSPEC TCart : public TObserver
        {
        public:
            void addProduct(TProduct* product)
            {
                products.push_back(product);
                product->attach(this);
            }
            void update(TProduct* product);
            size_t size()
            {
                return products.size();
            }
        };

        class DECLSPEC TCheckout : public TObserver
        {
        private:
            TMoney totalPrice;
        public:
            void update(TProduct* product);
            void addProduct(TProduct* product)
            {
                products.push_back(product);
                product->attach(this);
                update(product);
            }
            TMoney getTotalPrice()
            {
                return totalPrice;
            }
        };
    }
}
