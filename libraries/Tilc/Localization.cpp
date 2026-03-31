#include "Tilc/Localization.h"
#include "Tilc/Utils/ExtString.h"

Tilc::TLocalization* Loc = nullptr;

void Tilc::TLocalization::InitLocalization()
{
	Tilc::TLangMap o;

	m_LangMaps["pl"] = o;
	m_LangMaps["en"] = o;
}
