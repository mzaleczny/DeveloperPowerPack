#pragma once

#include "Tilc/DllGlobals.h"
#include "Tilc/Utils/ExtString.h"
#include "Tilc/Commerce/Money.h"
#include <vector>
#include <algorithm>
#include <unordered_map>

namespace Tilc
{
    class TStdObject;

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

        class DECLSPEC TDataObject
        {
        public:
            Tilc::TExtString TableName;
            Tilc::TExtString ListLabel;
            Tilc::TExtString AddLabel;
            Tilc::TExtString EditLabel;
            int64_t Tag;
            virtual std::vector<const char*> GetAllFieldsNamesList() = 0;
            inline Tilc::TExtString GetAllFieldsNames()
            {
                return Tilc::Implode(',', GetAllFieldsNamesList());
            }
            virtual Tilc::TExtString ToJson() = 0;
            virtual void FromJson(const Tilc::TExtString& JsonContent) = 0;
            virtual void FromJsonObject(Tilc::TStdObject* JsonObject) = 0;
            // Returned pointer **MUST** be deleted: delete obj;
            Tilc::TStdObject* GetDataObject(const Tilc::TExtString& JsonContent);
            virtual bool IsEmpty() const = 0;
            virtual std::unordered_map<Tilc::TExtString, Tilc::TExtString> GetDataMap() const = 0;
            virtual void SetDataFromMap(std::unordered_map<Tilc::TExtString, Tilc::TExtString> Data) = 0;
            virtual std::vector<Tilc::TExtString> GetDataForListColumns() const = 0;
        };

        class DECLSPEC TCategory : public TDataObject
        {
        public:
            Tilc::TExtString id;
            Tilc::TExtString name;
            Tilc::TExtString slug;
            Tilc::TExtString short_description;
            Tilc::TExtString description;

            TCategory()
            {
                TableName = "categories";
                ListLabel = "Lista kategorii";
                AddLabel = "Dodaj nową kategorię";
                EditLabel = "Edytuj kategorię";
            }

            std::vector<const char*> GetAllFieldsNamesList() override
            {
                return {"id", "name", "slug", "short_description", "description"};
            }
            Tilc::TExtString ToJson() override;
            void FromJson(const Tilc::TExtString& JsonContent) override;
            void FromJsonObject(Tilc::TStdObject* JsonObject) override;
            bool IsEmpty() const override;
            std::unordered_map<Tilc::TExtString, Tilc::TExtString> GetDataMap() const override;
            void SetDataFromMap(std::unordered_map<Tilc::TExtString, Tilc::TExtString> Data) override;
            std::vector<Tilc::TExtString> GetDataForListColumns() const override;
        };

        class DECLSPEC TProduct : public TDataObject
        {
        public:
            Tilc::TExtString id;
            Tilc::TExtString name;
            Tilc::TExtString slug;
            Tilc::TExtString name_en;
            Tilc::TExtString short_description;
            Tilc::TExtString description;
            TMoney price;
            TMoney price_1;
            TMoney price_2;
            TMoney price_3;
            Tilc::TExtString mini_map_file;
            Tilc::TExtString css_class;
            Tilc::TExtString product_code;
            Tilc::TExtString created;
            Tilc::TExtString modified;
            int inventoryLevel;
            std::vector<TObserver*> observers;
            std::vector<TReview*> reviews;
            double averageReviewScore;
            std::vector<Tilc::TExtString> pictures;

            // Constructor
            TProduct()
            {
                CommonInit();
            }

            void CommonInit()
            {
                TableName = "products";
                ListLabel = "Lista produktów";
                AddLabel = "Dodaj nowy produkt";
                EditLabel = "Edytuj produkt";
            }

            TProduct(Tilc::TExtString name, Tilc::TExtString slug, Tilc::TExtString short_description, int price, int inventoryLevel,
                    Tilc::TExtString mini_map_file = "", Tilc::TExtString css_class = "", Tilc::TExtString product_code = "",
                    Tilc::TExtString created = "", Tilc::TExtString modified = "")
                : name(name), slug(slug), short_description(short_description), price(price), mini_map_file(mini_map_file), css_class(css_class), product_code(product_code),
                  created(created), modified(modified), inventoryLevel(inventoryLevel), averageReviewScore(0.0)
            {
                CommonInit();
            }

            TProduct(Tilc::TExtString name, Tilc::TExtString slug, Tilc::TExtString short_description, double price, int inventoryLevel,
                    Tilc::TExtString mini_map_file = "", Tilc::TExtString css_class = "", Tilc::TExtString product_code = "",
                    Tilc::TExtString created = "", Tilc::TExtString modified = "")
                : TProduct(name, slug, short_description, 0, inventoryLevel, mini_map_file, css_class, product_code,
                  created, modified)
            {
                this->price = price;
            }

            std::vector<const char*> GetAllFieldsNamesList() override
            {
                return {"id", "name", "slug", "short_description", "description", "price", "price_1", "price_2", "price_3", "mini_map_file", "css_class", "product_code", "created", "modified"};
            }
            Tilc::TExtString ToJson() override;
            void FromJson(const Tilc::TExtString& JsonContent) override;
            void FromJsonObject(Tilc::TStdObject* JsonObject) override;
            bool IsEmpty() const override;
            std::unordered_map<Tilc::TExtString, Tilc::TExtString> GetDataMap() const override;
            void SetDataFromMap(std::unordered_map<Tilc::TExtString, Tilc::TExtString> Data) override;
            std::vector<Tilc::TExtString> GetDataForListColumns() const override;



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


    class DECLSPEC TUser : public Commerce::TDataObject
    {
    public:
        Tilc::TExtString id;
        Tilc::TExtString name;
        Tilc::TExtString email;
        Tilc::TExtString sub;
        Tilc::TExtString picture;
        Tilc::TExtString error;

        TUser()
        {
            TableName = "users";
            ListLabel = "Lista użytkowników";
            AddLabel = "Dodaj nowego użytkownika";
            EditLabel = "Edytuj użytkownika";
        }

        std::vector<const char*> GetAllFieldsNamesList() override
        {
            return {"id", "name", "email", "sub", "picture"};
        }
        Tilc::TExtString ToJson() override;
        void FromJson(const Tilc::TExtString& JsonContent) override;
        void FromJsonObject(Tilc::TStdObject* JsonObject) override;
        bool IsEmpty() const override;
        std::unordered_map<Tilc::TExtString, Tilc::TExtString> GetDataMap() const override;
        void SetDataFromMap(std::unordered_map<Tilc::TExtString, Tilc::TExtString> Data) override;
        std::vector<Tilc::TExtString> GetDataForListColumns() const override;
    };
}
