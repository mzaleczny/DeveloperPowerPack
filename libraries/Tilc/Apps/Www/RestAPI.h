#pragma once
#include "Tilc/Utils/ExtString.h"

#ifdef __EMSCRIPTEN__
#include "Tilc/Apps/Www/RestAPI_Emscripten.h"
#else
#include "Tilc/Apps/Www/RestAPI_Curl.h"
#endif
