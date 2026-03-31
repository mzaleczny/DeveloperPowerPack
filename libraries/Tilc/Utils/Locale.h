#pragma once

#include "Tilc/DllGlobals.h"
#include "Tilc/Utils/ExtString.h"

namespace Tilc
{

	class DECLSPEC TLocale
	{
	public:
		TLocale(const char* LocaleName = nullptr);
		virtual ~TLocale();

		char m_LocaleSdecimal[4];

	private:
		TExtString m_LocaleName;
	};

	extern TLocale* UserLocale;

}
