#pragma once

#ifdef __EMSCRIPTEN__

#include <emscripten/fetch.h>
#include "Tilc/Utils/ExtString.h"
#include <iostream>
#include <functional>

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
                auto context = new std::function<void(Tilc::TExtString)>(onComplete);

                emscripten_fetch_attr_t attr;
                emscripten_fetch_attr_init(&attr);

                strcpy(attr.requestMethod, "GET");
                attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
                attr.userData = context; // Przekazujemy wskaźnik do naszej funkcji

                attr.onsuccess = [](emscripten_fetch_t* fetch) {
                    auto cb = static_cast<std::function<void(Tilc::TExtString)>*>(fetch->userData);
                    std::cout << "[C++ WASM] Pobrano " << fetch->numBytes << " bajtów z API.\n";

                    Tilc::TExtString json(fetch->data, fetch->numBytes);
                    (*cb)(json);

                    delete cb;
                    emscripten_fetch_close(fetch);
                };

                attr.onerror = [](emscripten_fetch_t* fetch) {
                    auto cb = static_cast<std::function<void(Tilc::TExtString)>*>(fetch->userData);
                    std::cout << "[C++ WASM] Zapytanie do REST API nie powiodło się, status: "
                              << fetch->status << std::endl;

                    delete cb;
                    emscripten_fetch_close(fetch);
                };

                emscripten_fetch(&attr, Url.c_str());
            }

            template <typename Callback>
            inline void DoPostAsync(Tilc::TExtString Url, Tilc::TExtString JsonPayload, Callback onComplete, std::vector<Tilc::TExtString> Headers = {})
            {
                // 1. Tworzymy strukturę, która przechowa ZARÓWNO callback, JAK I kopię danych payloadu
                struct FetchContext {
                    std::function<void(Tilc::TExtString)> callback;
                    std::string payloadData; // Trzymamy dane w pamięci tak długo, jak żyje context
                };

                auto context = new FetchContext{
                    onComplete,
                    std::string(JsonPayload.c_str(), JsonPayload.length()) // Kopia zapasowa danych
                };

                emscripten_fetch_attr_t attr;
                emscripten_fetch_attr_init(&attr);

                strcpy(attr.requestMethod, "POST");
                attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
                attr.userData = context; // Przekazujemy wskaźnik do naszej struktury

                std::vector<const char*> Hdrs[] = {
                    "Content-Type", "application/json",
                };
                for (auto Hdr : Headers)
                {
                    Hdrs.push_back(Hdr.c_str());
                }
                Hdrs.push_back(nullptr);
                attr.requestHeaders = headers.data();

                // 2. Wskazujemy na dane z BEZPIECZNEGO obiektu w context (żyjącego na stercie)
                attr.requestData = context->payloadData.c_str();
                attr.requestDataSize = context->payloadData.length();

                // 3. Callbacki
                attr.onsuccess = [](emscripten_fetch_t* fetch) {
                    auto ctx = static_cast<FetchContext*>(fetch->userData);

                    Tilc::TExtString json(fetch->data, fetch->numBytes);

                    // Wywołujemy callback użytkownika
                    if (ctx->callback) {
                        ctx->callback(json);
                    }

                    // Zwalniamy context DOPERO TERAZ (razem z buforem payloadData)
                    delete ctx;
                    emscripten_fetch_close(fetch);
                };

                attr.onerror = [](emscripten_fetch_t* fetch) {
                    auto ctx = static_cast<FetchContext*>(fetch->userData);

                    std::cout << "[C++ WASM] Zapytanie do REST API nie powiodło się, status: "
                              << fetch->status << std::endl;

                    // Zwalniamy context w przypadku błędu
                    delete ctx;
                    emscripten_fetch_close(fetch);
                };

                // 4. Wysyłanie zapytania
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
