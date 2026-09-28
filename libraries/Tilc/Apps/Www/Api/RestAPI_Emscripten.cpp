#ifdef __EMSCRIPTEN__

#include "Tilc/Apps/Www/RestAPI.h"
#include <iostream>

void Tilc::Apps::Www::DownloadSucceeded(emscripten_fetch_t *fetch)
{
    std::cout << "[C++ WASM] Pobrano " << fetch->numBytes << " bajtów z API.\n";

    // Dane odpowiedzi znajdują się w fetch->data
    Tilc::TExtString response(fetch->data, fetch->numBytes);
    std::cout << "Treść JSON: " << response << std::endl;

    emscripten_fetch_close(fetch); // Zwolnienie zasobów zapytania
}

void Tilc::Apps::Www::DownloadFailed(emscripten_fetch_t *fetch)
{
    std::cout << "[C++ WASM] Zapytanie do REST API nie powiodło się, status: "
              << fetch->status << std::endl;

    emscripten_fetch_close(fetch);
}

void Tilc::Apps::Www::FetchCategories()
{
    emscripten_fetch_attr_t attr;
    emscripten_fetch_attr_init(&attr);

    strcpy(attr.requestMethod, "GET");
    attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
    attr.onsuccess = DownloadSucceeded;
    attr.onerror = DownloadFailed;

    // Opcjonalne nagłówki JSON dla POST:
    // const char* headers[] = {"Content-Type", "application/json", NULL};
    // attr.requestHeaders = headers;

    emscripten_fetch(&attr, RestApiUrl_CategoriesList.c_str());
}

#endif
