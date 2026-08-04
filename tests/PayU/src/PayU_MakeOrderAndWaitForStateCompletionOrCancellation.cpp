#include <iostream>
#include <thread>
#include "Tilc/Commerce/PayU.h"
#include "Tilc/Commerce/Shop.h"
#include "Tilc/Utils/ExtString.h"
#include "Tilc/Utils/JsonParser.h"
#include "Tilc/Utils/StdObject.h"
#ifdef _WIN32
    #include <Windows.h>
#endif

int main(int argc, char* argv[])
{
    Tilc::Commerce::TPayU p;
    p.SetShopDescription("Tilc Mega Store");
    
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



    // Create a product with an initial inventory level of 10
    std::vector<Tilc::Commerce::TProduct> products{
        Tilc::Commerce::TProduct("TV", "tv", "A 55-inch 4K smart TV.", 200.00, 1),
        Tilc::Commerce::TProduct("Computer", "computer", "A gaming machine", 300.00, 1),
        Tilc::Commerce::TProduct("Keyborad", "keyboard", "A keyboard for computer", 100.00, 1),
    };

    // Create a shopping cart and add the product to the cart
    Tilc::Commerce::TCart cart = Tilc::Commerce::TCart();
    cart.addProduct(&products[0]);
    cart.addProduct(&products[1]);
    cart.addProduct(&products[2]);

    // Create a checkout object and add it as an observer of the product
    Tilc::Commerce::TCheckout checkout = Tilc::Commerce::TCheckout();
    // products vector in cart and checkout is shared by both classes: TCart and TCheckout sa here we only attach observer and not add products to it
    for (auto& p : products)
    {
        p.attach(&checkout);
    }

    // Reduce the inventory level of the product and observe the effect on the cart and checkout
    products[2].setInventoryLevel(5);
    //std::cout << "Cart size: " << cart.size() << std::endl;
    //std::cout << "Checkout Total price: " << checkout.getTotalPrice().ToString() << std::endl;

    // Reduce the inventory level of the product to zero and observe the effect on the cart and checkout
    products[2].setInventoryLevel(0);
    //std::cout << "Cart size: " << cart.size() << std::endl;
    //std::cout << "Checkout Total price: " << checkout.getTotalPrice().ToString() << std::endl;


    Tilc::TExtString Result, JsonString, RedirectUri, PayUOrderId;
    JsonString = p.Login();
    //std::cout << "Bearer: " << p.GetBearer() << std::endl << std::endl;
    Result = p.MakeOrder(&cart, &checkout, "127.0.0.1", "https://appsoft.cc/shop/pl/order-placed", "https://appsoft.cc/shop/pl/notify", std::to_string(time(nullptr)), RedirectUri, PayUOrderId);
    if (Result.StartsWith("ERROR:"))
    {
        std::cout << Result << std::endl;
        return -1;
    }

    std::cout << "Redirecting to: " << RedirectUri << std::endl;
    std::cout << "PayU order id: " << PayUOrderId << std::endl;

    if (!PayUOrderId.empty())
    {
        // Run uri in webbrowser to allow user to pay order
#ifdef _WIN32
        ShellExecuteA(
            NULL,
            "open",
            "c:\\Program Files\\Mozilla Firefox\\firefox.exe",
            RedirectUri.c_str(),
            NULL,
            SW_SHOWNORMAL
        );
#else
        system(("firefox " + RedirectUri).c_str());
#endif

        Tilc::TStdObject* Order = nullptr;
        bool DoLoop = true;
        while (DoLoop)
        {
            Result = p.RetrieveOrder(PayUOrderId, &Order);
            if (Result == "OK")
            {
                Result = Order->getAsObject("root")->getAsString("status");
                std::cout << "Status: " << Result << std::endl;
                if (Result == "COMPLETED" || Result == "CANCELED")
                {
                    if (Result == "COMPLETED")
                    {
                        std::cout << "Transaction successfully paid, updating database." << std::endl;
                    }
                    else
                    {
                        std::cout << "Transaction canceled, updating database." << std::endl;
                    }
                    DoLoop = false;
                }
            }
            if (Order)
            {
                delete Order;
                Order = nullptr;
            }

            if (DoLoop)
            {
                // Sleep for 3 seconds and repeat retrieving order data
                std::this_thread::sleep_for(std::chrono::seconds(10));
            }
        }

        std::cout << "Completed" << std::endl;
    }

    return 0;
}
