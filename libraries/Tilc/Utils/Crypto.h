#pragma once
#include "Tilc/DllGlobals.h"

namespace Tilc
{
	DECLSPEC char* StringXor(char* str, size_t len, const char c);
	DECLSPEC char* StringXorStr(char* str, size_t len, const char* chars);
}
