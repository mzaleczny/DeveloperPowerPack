#pragma once

#ifdef __EMSCRIPTEN__
#include <string>
#include <vector>
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
            inline void FetchAsync(Tilc::TExtString Url, Callback onComplete, std::vector<Tilc::TExtString> Headers = {})
            {
                struct FetchContext {
                    std::function<void(Tilc::TExtString)> callback;
                    std::string payloadData; // Trzymamy dane w pamięci tak długo, jak żyje context
                    std::vector<std::string> headers;
                    std::vector<const char*> c_headers;
                };

                // Tworzymy strukturę danych do przekazania do callbacku
                auto context = new FetchContext{
                    onComplete
                };

                emscripten_fetch_attr_t attr;
                emscripten_fetch_attr_init(&attr);

                strcpy(attr.requestMethod, "GET");
                attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
                attr.userData = context; // Przekazujemy wskaźnik do naszej funkcji

                context->headers.push_back("Content-Type: application/json");
                for (auto Hdr : Headers)
                {
                    Tilc::TExtString trimmed = Hdr;
                    trimmed.Trim();
                    if (!trimmed.empty())
                    {
                        context->headers.push_back(trimmed);
                    }
                }
                context->c_headers.reserve(context->headers.size() + 1);
                for (auto& Hdr : context->headers)
                {
                    context->c_headers.push_back(Hdr.c_str());
                }
                context->c_headers.push_back(nullptr);
                attr.requestHeaders = context->c_headers.data();

                attr.onsuccess = [](emscripten_fetch_t* fetch) {
                    Tilc::TExtString json(fetch->data, fetch->numBytes);
                    auto ctx = static_cast<FetchContext*>(fetch->userData);
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
                    // Zwalniamy context w przypadku błędu
                    delete ctx;
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
                    std::vector<std::string> headers;
                    std::vector<const char*> c_headers;
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

                context->headers.push_back("Content-Type: application/json");
                for (auto Hdr : Headers)
                {
                    Tilc::TExtString trimmed = Hdr;
                    trimmed.Trim();
                    if (!trimmed.empty())
                    {
                        context->headers.push_back(trimmed);
                    }
                }
                context->c_headers.reserve(context->headers.size() + 1);
                for (auto& Hdr : context->headers)
                {
                    context->c_headers.push_back(Hdr.c_str());
                }
                context->c_headers.push_back(nullptr);
                attr.requestHeaders = context->c_headers.data();

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
            void Save(Tilc::TExtString Url, std::vector<T>& Items, Callback onComplete, Tilc::TExtString LoggedUser, std::vector<Tilc::TExtString> Headers = {})
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
                    "LoggedUser": "{LOGGED_USER}",
                    "Items": [{ITEMS}]
                })##";
                Json.StrReplace("{ITEMS}", JsonItems);
                Json.StrReplace("{LOGGED_USER}", LoggedUser);
                DoPostAsync(Url, Json, onComplete, Headers);
            }

            template <typename Callback>
            void Delete(Tilc::TExtString Url, std::vector<int>& Ids, Callback onComplete, Tilc::TExtString LoggedUser, std::vector<Tilc::TExtString> Headers = {})
            {
                Tilc::TExtString Json;
                Json = R"##(
                {
                    "Action": "Delete",
                    "LoggedUser": "{LOGGED_USER}",
                    "Ids": [{IDS}]
                })##";
                Json.StrReplace("{IDS}", Tilc::Implode(',', Ids));
                Json.StrReplace("{LOGGED_USER}", LoggedUser);
                DoPostAsync(Url, Json, onComplete, Headers);
            }

        }
    }
}

#endif
