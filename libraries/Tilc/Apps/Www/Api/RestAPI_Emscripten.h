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
            void DownloadSucceeded(emscripten_fetch_t *fetch);
            void DownloadFailed(emscripten_fetch_t *fetch);
            void FetchCategories();
        }
    }
}

#endif
