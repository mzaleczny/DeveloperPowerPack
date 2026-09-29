#pragma once

#ifdef __EMSCRIPTEN__

#include <emscripten/fetch.h>
#include "Tilc/Utils/ExtString.h"

namespace Tilc
{
    namespace Apps
    {
        namespace Www
        {
            template <typename Callback>
            inline void FetchAsync(Tilc::TExtString Url, Callback onComplete)
            {
                // Tworzymy strukturę danych do przekazania do callbacku
                auto context = new std::function<void(std::string)>(onComplete);

                emscripten_fetch_attr_t attr;
                emscripten_fetch_attr_init(&attr);

                strcpy(attr.requestMethod, "GET");
                attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
                attr.userData = context; // Przekazujemy wskaźnik do naszej funkcji

                attr.onsuccess = [](emscripten_fetch_t* fetch) {
                    auto cb = static_cast<std::function<void(std::string)>*>(fetch->userData);
                    std::cout << "[C++ WASM] Pobrano " << fetch->numBytes << " bajtów z API.\n";

                    Tilc::TExtString json(fetch->data, fetch->numBytes);
                    (*cb)(json);

                    delete cb;
                    emscripten_fetch_close(fetch);
                };

                attr.onerror = [](emscripten_fetch_t* fetch) {
                    auto cb = static_cast<std::function<void(std::string)>*>(fetch->userData);
                    std::cout << "[C++ WASM] Zapytanie do REST API nie powiodło się, status: "
                              << fetch->status << std::endl;

                    delete cb;
                    emscripten_fetch_close(fetch);
                };

                emscripten_fetch(&attr, Url.c_str());
            }

            template <typename Callback>
            inline void DoPostAsync(Tilc::TExtString Url, Tilc::TExtString JsonPayload, Callback onComplete)
            {
                // Tworzymy strukturę danych do przekazania do callbacku
                auto context = new std::function<void(std::string)>(onComplete);

                emscripten_fetch_attr_t attr;
                emscripten_fetch_attr_init(&attr);

                // 1. Ustawienie metody
                strcpy(attr.requestMethod, "POST");

                // 2. Wskazanie flagi ładowania do pamięci
                attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;

                // 3. Nagłówki zapytania (musi kończyć się NULL)
                const char* headers[] = {
                    "Content-Type", "application/json",
                    NULL
                };
                attr.requestHeaders = headers;

                // 4. Treść wiadomości (Body) i jej rozmiar
                attr.requestData = JsonPayload.c_str();
                attr.requestDataSize = JsonPayload.length();

                // 5. Rejestracja callbacków

                attr.onsuccess = [](emscripten_fetch_t* fetch) {
                    auto cb = static_cast<std::function<void(std::string)>*>(fetch->userData);
                    std::cout << "[C++ WASM] Pobrano " << fetch->numBytes << " bajtów z API.\n";

                    Tilc::TExtString json(fetch->data, fetch->numBytes);
                    (*cb)(json);

                    delete cb;
                    emscripten_fetch_close(fetch);
                };

                attr.onerror = [](emscripten_fetch_t* fetch) {
                    auto cb = static_cast<std::function<void(std::string)>*>(fetch->userData);
                    std::cout << "[C++ WASM] Zapytanie do REST API nie powiodło się, status: "
                              << fetch->status << std::endl;

                    delete cb;
                    emscripten_fetch_close(fetch);
                };

                // 6. Wysyłanie zapytania w tle
                emscripten_fetch(&attr, Url.c_str());
            }

            template <typename T, typename Callback>
            void Save(Tilc::TExtString Url, std::vector<T>& Items, Callback onComplete)
            {
                Tilc::TExtString Json;
                Tilc::TExtString JsonItems;

                for (size_t i = 0; i < Items.size(); ++i)
                {
                    if (i > 0) JsonItems += ",";
                    JsonItems += Items[i].ToJson();
                }
                Json = R"##(
                {
                    "Action": "Save",
                    "Items": [{ITEMS}]
                })##";
                Json.StrReplace("{ITEMS}", JsonItems);
                DoPostAsync(Url, Json, onComplete);
            }

            template <typename Callback>
            void Delete(Tilc::TExtString Url, std::vector<int>& Ids, Callback onComplete)
            {
                Tilc::TExtString Json;
                Json = R"##(
                {
                    "Action": "Delete",
                    "Ids": [{IDS}]
                })##";
                Json.StrReplace("{IDS}", Tilc::Implode(',', Ids));
                DoPostAsync(Url, Json, onComplete);
            }

        }
    }
}

#endif
