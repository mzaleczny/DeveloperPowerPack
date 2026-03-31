#pragma once
#include "Tilc/DllGlobals.h"
#include "Tilc/Utils/ExtString.h"
#include <map>

#define _L(TEXT)  ((Loc) ? Loc->m_LangMaps[Loc->m_Lang][(TEXT)] : "")

namespace Tilc
{
	using TLangMap = std::map<Tilc::TExtString, Tilc::TExtString>;

	class DECLSPEC TLocalization
	{
	public:
		Tilc::TExtString m_Lang;
		std::map<Tilc::TExtString, TLangMap> m_LangMaps;
		virtual void InitLocalization();
	};
}

extern DECLSPEC Tilc::TLocalization* Loc;
