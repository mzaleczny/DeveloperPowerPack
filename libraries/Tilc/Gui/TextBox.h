#pragma once

#include "Tilc/DllGlobals.h"
#include <Tilc/Utils/ExtString.h>
#include <vector>
#include <Tilc/Gui/GuiControl.h>

struct SDL_Renderer;
struct TTF_Font;

namespace Tilc::Gui
{

	enum class ETextLinesAlignment
	{
		ETLA_Top,
		ETLA_Bottom
	};

	class TFont;

	class DECLSPEC TTextBox : public TGuiControl
	{
	public:
		TTextBox(TGuiControl* parent, Tilc::TExtString name, const SDL_FRect& position, const char* Content, size_t ContentLength, TFont* ExistingFont, float FontSize);
		TTextBox(TGuiControl* parent, Tilc::TExtString name, const SDL_FRect& position, const char* Content, size_t ContentLength, TTF_Font* ExistingFont, float FontSize);
		TTextBox(TGuiControl* parent, Tilc::TExtString name, const SDL_FRect& position, const char* DbResourcesFname, const char* ResourceName, const char* FontResourceName, float FontSize);
		~TTextBox();
		virtual void Draw() override;
		Tilc::Gui::TFont* GetFont() { return Font; }
		void SetText(const char* Content);
	protected:
		Tilc::Gui::TFont* Font = nullptr;
		std::vector<Tilc::TExtString> ContentLines;
		Tilc::Gui::ETextLinesAlignment Alignment;
		int ColorR = 0xff;
		int ColorG = 0xff;
		int ColorB = 0xff;
		bool ReleaseFont = true;
		void CreateContentLinesFromString(const char* Content, size_t ContentLength = 0);
	};
}
