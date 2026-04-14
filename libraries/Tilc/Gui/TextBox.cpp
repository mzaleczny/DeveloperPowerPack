#include "Tilc/Gui/TextBox.h"
#include "Tilc/Utils/ExtString.h"
#include "Tilc/Gui/Font.h"
#include "Tilc/Data/DataDbResources.h"
#include "Tilc/Utils/ExtString.h"

Tilc::Gui::TTextBox::TTextBox(TGuiControl* parent, Tilc::TExtString name, const SDL_FRect& position, const char* Content, size_t ContentLength, TFont* ExistingFont, float FontSize)
	: Tilc::Gui::TGuiControl(parent, name, position, Tilc::Gui::EControlType::ECT_RegularControl, false)
{
	Alignment = Tilc::Gui::ETextLinesAlignment::ETLA_Top;
	ColorR = 0xff;
	ColorG = 0xff;
	ColorB = 0xff;

	CreateContentLinesFromString(Content, ContentLength);
	Font = ExistingFont;
	ReleaseFont = false;
}

Tilc::Gui::TTextBox::TTextBox(TGuiControl* parent, Tilc::TExtString name, const SDL_FRect& position, const char* Content, size_t ContentLength, TTF_Font* ExistingFont, float FontSize)
    : Tilc::Gui::TGuiControl(parent, name, position, Tilc::Gui::EControlType::ECT_RegularControl, false)
{
	Alignment = Tilc::Gui::ETextLinesAlignment::ETLA_Top;
	ColorR = 0xff;
	ColorG = 0xff;
	ColorB = 0xff;

	CreateContentLinesFromString(Content, ContentLength);
	Font = new Tilc::Gui::TFont(ExistingFont);
	ReleaseFont = true;
}

Tilc::Gui::TTextBox::TTextBox(TGuiControl* parent, Tilc::TExtString name, const SDL_FRect& position, const char* DbResourcesFname, const char* ResourceName, const char* FontResourceName, float FontSize)
    : Tilc::Gui::TGuiControl(parent, name, position, Tilc::Gui::EControlType::ECT_RegularControl, false)
{
	char* Buffer = nullptr;
	size_t BufferSize;
	Tilc::TExtString ResOrigPath;

	Alignment = Tilc::Gui::ETextLinesAlignment::ETLA_Top;
	ColorR = 0xff;
	ColorG = 0xff;
	ColorB = 0xff;

	if (Tilc::Data::Data)
	{
		Tilc::Data::Data->GetResourceByName(DbResourcesFname, ResourceName, ResOrigPath, &Buffer, &BufferSize, 0);
		if (Buffer)
		{
			CreateContentLinesFromString(Buffer, BufferSize);
			delete[] Buffer;
			Buffer = nullptr;
		}
	}
	Font = new Tilc::Gui::TFont(FontResourceName, FontSize, false);
	ReleaseFont = true;
}

void Tilc::Gui::TTextBox::CreateContentLinesFromString(const char* Content, size_t ContentLength)
{
	Tilc::TExtString Settings;
	std::vector<Tilc::TExtString> Lines;
	if (ContentLength == 0)
	{
		ContentLength = strlen(Content);
	}
	Settings.assign(Content, ContentLength);
	Settings.Explode('\n', Lines);
	for (int i = 0; i < Lines.size(); i++)
	{
		if (Lines[i] == "ALIGN_TEXT_LINE:TOP")
		{
			Alignment = Tilc::Gui::ETextLinesAlignment::ETLA_Top;
			continue;
		}
		if (Lines[i].find("TEXT_LINE_COLOR:") != Tilc::TExtString::npos)
		{
			Tilc::TExtString HexColor = Lines[i].substr(16);
			Tilc::TExtString Byte;
			if (HexColor[0] == '#')
			{
				HexColor = HexColor.substr(1);
			}
			Byte = HexColor.substr(0, 2);
			ColorR = std::stoi(Byte, 0, 16);
			Byte = HexColor.substr(2, 2);
			ColorG = std::stoi(Byte, 0, 16);
			Byte = HexColor.substr(4, 2);
			ColorB = std::stoi(Byte, 0, 16);
			continue;
		}
		ContentLines.push_back(Lines[i]);
	}
}

Tilc::Gui::TTextBox::~TTextBox()
{
	if (ReleaseFont && Font)
	{
		delete Font;
		Font = nullptr;
	}
}

void Tilc::Gui::TTextBox::Draw()
{
    SDL_Renderer* Renderer = Tilc::GameObject->GetContext()->m_Window->GetRenderer();
	if (ContentLines.size() > 0)
	{
		SDL_Color BlackColor = { 0x00, 0x00, 0x00, 0xff };
		SDL_Color WhiteColor = { 0xff, 0xff, 0xff, 0xff };

		SDL_Color FontColor;
		FontColor.r = ColorR;
		FontColor.g = ColorG;
		FontColor.b = ColorB;
		FontColor.a = 255;

		if (Alignment == ETextLinesAlignment::ETLA_Top)
		{
			float x = /*m_Backdrop.x*/ + 20.0f;
			float y = /*m_Backdrop.y*/ + 20.0f;
			float LineHeight = Font->GetSize() * 1.5f;
			for (int i = 0; i < ContentLines.size(); i++)
			{
				Font->SetColor(BlackColor);
				Font->DrawString(Renderer, ContentLines[i].c_str(), x - 2.0f, y + 2.0f);
				Font->SetColor(FontColor);
				Font->DrawString(Renderer, ContentLines[i].c_str(), x, y);
				y += LineHeight;
			}
		}
		else if (Alignment == ETextLinesAlignment::ETLA_Bottom)
		{
			/*
			float x = 0.0f;
			float y = ParentApplication->GetScreenHeight() - 70.0f - 20.0f;
			for (int i = Lines.size() - 1; i >= 0; i--)
			{
				Font->GetXCoordForCenteredText(Lines[i].c_str(), x);
				Font->SetColor(BlackColor);
				Font->DrawString(x - 2.0f, y + 2.0f, Lines[i].c_str(), true);
				Font->SetColor(FontColor);
				Font->DrawString(x, y, Lines[i].c_str(), true);
				y -= 70.0f;
			}
			*/
		}
	}
}

void Tilc::Gui::TTextBox::SetText(const char* Content)
{
	ContentLines.clear();
	CreateContentLinesFromString(Content, strlen(Content));
}
