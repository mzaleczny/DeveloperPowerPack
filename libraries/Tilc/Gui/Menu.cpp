#include "Tilc/Gui/Menu.h"
#include "Tilc/Gui/Font.h"
#include "Tilc/Game.h"
#include "Tilc/Window.h"
#include "Tilc/Utils/ExtString.h"
#include "Tilc/Utils/Vector.h"
#include "Tilc/Data/DataDbResources.h"
#include "Tilc/Audio/Music.h"
#include "Tilc/Graphics/TextRenderer.h"
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>


int Tilc::Gui::TMenu::MenuCounter = 0;

Tilc::Gui::TMenuItem::TMenuItem(const char* Caption, bool Available, bool DrawIcon)
{
	m_Caption = Caption;
	m_Available = Available;
	m_DrawIcon = DrawIcon;
}

Tilc::Gui::TMenu::TMenu(TGuiControl* parent, const Tilc::TExtString& name, const SDL_FRect& position, TFont* MenuFont, float SizeSelected)
    : TGuiControl(parent, name, position, Tilc::Gui::EControlType::ECT_Menu), m_Font(MenuFont)
{
    ++MenuCounter;
}

Tilc::Gui::TMenu::TMenu(TGuiControl* parent, const Tilc::TExtString& name, const SDL_FRect& position, TFont* MenuFont, bool OpenGLVersion, int SurfaceWidth, int SurfaceHeight)
    : TGuiControl(parent, name, position, Tilc::Gui::EControlType::ECT_Menu), m_Font(MenuFont)
{
    ++MenuCounter;
    Tilc::TExtString TextureResourceName = "Menu" + std::to_string(MenuCounter);
    m_TextContentForOpenGL = new Tilc::Graphics::TTextRenderer(TextureResourceName, SurfaceWidth, SurfaceHeight);
    if (m_TextContentForOpenGL)
    {
        m_TextContentForOpenGL->SetStandard2DRenderConfig("", "");
    }
}

Tilc::Gui::TMenu::TMenu(TGuiControl* parent, const Tilc::TExtString& name, const SDL_FRect& position, const char* MenuResourceName, const char* FontResourceName, float Size, float SizeSelected)
    : TGuiControl(parent, name, position, Tilc::Gui::EControlType::ECT_Menu)
{
	char* Buffer;
	size_t BufferSize;
	Tilc::TExtString ResOrigPath;
	const char* GameDataDbFile = Tilc::Data::Data->GetDbFname();
	Tilc::TExtString MenuBackgroundResourceName, MenuEnabledImageResourceName, MenuDisabledImageResourceName;

    ++MenuCounter;
    m_Font = Tilc::GameObject->AddFont(FontResourceName, FontResourceName, Size, false);

	// Load menu items
	Tilc::Data::Data->GetResourceByName(GameDataDbFile, MenuResourceName, ResOrigPath, &Buffer, &BufferSize, 0);
	if (Buffer)
	{
		int PosX = 0, PosY = 0;
		bool DrawMenuBg = false;
		Tilc::TExtString MenuItemColor{ "#ffffff" }, MenuItemColorHover{ "#ffff00" };
		int Alpha = 80;
		Tilc::TExtString MenuItems;
		MenuItems.assign(Buffer, BufferSize);
		delete[] Buffer;
		Buffer = nullptr;

		// Load menu items options ex. if it has background below it, and its position
		Tilc::Data::Data->GetResourceByName(GameDataDbFile, MenuResourceName, ResOrigPath, &Buffer, &BufferSize, 1);
		Tilc::TExtString MenuOptions;
		if (Buffer)
		{
			MenuOptions.assign(Buffer, BufferSize);
			delete[] Buffer;
			Buffer = nullptr;
			DrawMenuBg = (MenuOptions.find("HasBackground:TRUE") != Tilc::TExtString::npos) ? true : false;

			std::vector<Tilc::TExtString> Lines;
			MenuOptions.Explode('\n', Lines);
			for (int i = 0; i < Lines.size(); i++)
			{
				if (Lines[i].find("PosX:") != Tilc::TExtString::npos)
				{
					PosX = std::atoi(Lines[i].substr(Lines[i].find("PosX:") + 5).c_str());
				}
				if (Lines[i].find("PosY:") != Tilc::TExtString::npos)
				{
					PosY = std::atoi(Lines[i].substr(Lines[i].find("PosY:") + 5).c_str());
				}
				if (Lines[i].find("BgAlpha:") != Tilc::TExtString::npos)
				{
					Alpha = std::atoi(Lines[i].substr(Lines[i].find("BgAlpha:") + 8).c_str());
					Alpha = SDL_clamp(Alpha, 0, 100);
				}
				if (Lines[i].find("MenuItemColor:") != Tilc::TExtString::npos)
				{
					MenuItemColor = Lines[i].substr(Lines[i].find("MenuItemColor:") + 14);
					if (MenuItemColor.length() != 6 && MenuItemColor.length() != 7)
					{
						MenuItemColor = "#ffffff";
					}
				}
				if (Lines[i].find("MenuItemColorHover:") != Tilc::TExtString::npos)
				{
					MenuItemColorHover = Lines[i].substr(Lines[i].find("MenuItemColorHover:") + 19);
					if (MenuItemColorHover.length() != 6 && MenuItemColorHover.length() != 7)
					{
						MenuItemColorHover = "#ffff00";
					}
				}
				if (Lines[i].find("ResourceImageEnabled:") != Tilc::TExtString::npos)
				{
					MenuEnabledImageResourceName = Lines[i].substr(Lines[i].find("ResourceImageEnabled:") + 21);
				}
				if (Lines[i].find("ResourceImageDisabled:") != Tilc::TExtString::npos)
				{
					MenuDisabledImageResourceName = Lines[i].substr(Lines[i].find("ResourceImageDisabled:") + 22);
				}
			}
		}

		if (MenuItems.length() > 0)
		{
			if (DrawMenuBg)
			{
				MenuBackgroundResourceName = "Gui_menu_black_bg.png";
			}

			std::vector<Tilc::TExtString> Lines;
			MenuItems.Explode('\n', Lines);
			for (int i = 0; i < Lines.size(); ++i)
			{
				AddItem(Lines[i].c_str());
			}
			SetPosition(static_cast<float>(PosX), static_cast<float>(PosY));
			SetAlpha(Alpha);
		}

		int r, g, b, a;
		Tilc::TExtString HexColor = MenuItemColor, Byte;
		if (HexColor[0] == '#') HexColor = HexColor.substr(1);
		Byte = HexColor.substr(0, 2);
		r = std::stoi(Byte, 0, 16);
		Byte = HexColor.substr(2, 2);
		g = std::stoi(Byte, 0, 16);
		Byte = HexColor.substr(4, 2);
		b = std::stoi(Byte, 0, 16);
		a = 255;
		SetColor(r, g, b, a);

		HexColor = MenuItemColorHover, Byte;
		if (HexColor[0] == '#') HexColor = HexColor.substr(1);
		Byte = HexColor.substr(0, 2);
		r = std::stoi(Byte, 0, 16);
		Byte = HexColor.substr(2, 2);
		g = std::stoi(Byte, 0, 16);
		Byte = HexColor.substr(4, 2);
		b = std::stoi(Byte, 0, 16);
		a = 255;
		SetColorSelected(r, g, b, a);
	}

	if (MenuBackgroundResourceName.length() > 0)
	{
		Tilc::Data::Data->GetResourceByName(Tilc::Data::Data->GetDbFname(), MenuBackgroundResourceName.c_str(), ResOrigPath,
			&Buffer, &BufferSize, 0);
		if (Buffer)
		{
			m_MenuBackground = IMG_LoadTexture_IO(Tilc::GameObject->m_Window->GetRenderer(), SDL_IOFromConstMem(Buffer, BufferSize), true);
			delete[] Buffer;
			Buffer = nullptr;
		}
	}

	if (MenuEnabledImageResourceName.length() > 0)
	{
		Tilc::Data::Data->GetResourceByName(Tilc::Data::Data->GetDbFname(), MenuEnabledImageResourceName.c_str(), ResOrigPath,
			&Buffer, &BufferSize, 0);
		if (Buffer)
		{
			m_MenuEnabledImage = IMG_LoadTexture_IO(Tilc::GameObject->m_Window->GetRenderer(), SDL_IOFromConstMem(Buffer, BufferSize), true);
			delete[] Buffer;
			Buffer = nullptr;
		}
	}

	if (MenuDisabledImageResourceName.length() > 0)
	{
		Tilc::Data::Data->GetResourceByName(Tilc::Data::Data->GetDbFname(), MenuDisabledImageResourceName.c_str(), ResOrigPath,
			&Buffer, &BufferSize, 0);
		if (Buffer)
		{
			m_MenuDisabledImage = IMG_LoadTexture_IO(Tilc::GameObject->m_Window->GetRenderer(), SDL_IOFromConstMem(Buffer, BufferSize), true);
			delete[] Buffer;
			Buffer = nullptr;
		}
	}

	/*
	Tilc::TExtString Utf8String("Dźwięk po kliknięciu pozycji menu");
	Utf8String.AnsiToUtf8();
	WavMenuActivated = new Tilc::Audio::TMusic(Utf8String.c_str());
	Utf8String.assign("Dźwięk po zmianie zaznaczenia pozycji menu");
	WavMenuSelected = new Tilc::Audio::TMusic(Utf8String.c_str());
	*/
}

Tilc::Gui::TMenu::~TMenu()
{
    if (m_TextContentForOpenGL)
    {
        delete m_TextContentForOpenGL;
        m_TextContentForOpenGL = nullptr;
    }
	if (m_WavMenuActivated)
	{
		delete m_WavMenuActivated;
        m_WavMenuActivated = nullptr;
	}
	if (m_WavMenuSelected)
	{
		delete m_WavMenuSelected;
        m_WavMenuSelected = nullptr;
	}
	if (m_MenuBackground)
	{
		SDL_DestroyTexture(m_MenuBackground);
        m_MenuBackground = nullptr;
	}
	if (m_MenuEnabledImage)
	{
		SDL_DestroyTexture(m_MenuEnabledImage);
        m_MenuEnabledImage = nullptr;
	}
	if (m_MenuDisabledImage)
	{
		SDL_DestroyTexture(m_MenuDisabledImage);
        m_MenuDisabledImage = nullptr;
	}
}

void Tilc::Gui::TMenu::AddItem(const char* MenuItem, bool Available, bool DrawIcon)
{
	int ItemWidth, ItemHeight;
	int AddSpacing = m_MenuItems.size() > 0 ? m_VSpacing : 0;

	TMenuItem Item(MenuItem, Available, DrawIcon);
    m_MenuItems.push_back(Item);

    m_Font->GetTextSize(MenuItem, ItemWidth, ItemHeight);
	if (ItemWidth > m_MenuWidth)
	{
        m_MenuWidth = ItemWidth;
	}
	
	if (ItemHeight > m_ItemHeight)
	{
        m_ItemHeight = ItemHeight;
	}

    m_MenuHeight += ItemHeight + AddSpacing;
    m_NeedRegenerateSurface = true;
}

void Tilc::Gui::TMenu::SetItem(int AtPos, const char* MenuItem, bool Available, bool DrawIcon)
{
    m_MenuItems[AtPos].m_Caption = MenuItem;
    m_MenuItems[AtPos].m_Available = Available;
    m_MenuItems[AtPos].m_DrawIcon = DrawIcon;
    m_NeedRegenerateSurface = true;
}

void Tilc::Gui::TMenu::InsertItem(int AtPos, const char* MenuItem, bool Available, bool DrawIcon)
{
	int ItemWidth, ItemHeight;
	int AddSpacing = m_MenuItems.size() > 0 ? m_VSpacing : 0;

	TMenuItem Item(MenuItem, Available, DrawIcon);
    m_MenuItems.insert(m_MenuItems.begin() + AtPos, Item);
    m_Font->GetTextSize(MenuItem, ItemWidth, ItemHeight);
	if (ItemWidth > m_MenuWidth)
	{
        m_MenuWidth = ItemWidth;
	}

	if (ItemHeight > m_ItemHeight)
	{
        m_ItemHeight = ItemHeight;
	}

    m_MenuHeight += ItemHeight + AddSpacing;
}

void Tilc::Gui::TMenu::Render()
{
    if (m_TextContentForOpenGL->m_NeedRegenerateBuffers)
    {
        Tilc::TVector2D<int> WindowSize = Tilc::GameObject->GetContext()->m_Window->GetWindowSize();
        m_TextContentForOpenGL->Clear();
        for (int i = 0; i < m_MenuItems.size(); ++i)
        {
            if (i == GetSelectedIndex())
            {
                m_TextContentForOpenGL->PushTextForRender16(m_PosX, m_PosY - i * ((m_ItemHeight + m_VSpacing) / static_cast<float>(WindowSize.y)) * 2.0f, m_MenuItems[i].m_Caption, m_Font->m_FontName, m_ColorSelected);
            }
            else
            {
                m_TextContentForOpenGL->PushTextForRender16(m_PosX, m_PosY - i * ((m_ItemHeight + m_VSpacing) / static_cast<float>(WindowSize.y)) * 2.0f, m_MenuItems[i].m_Caption, m_Font->m_FontName, m_Color);
            }
        }
    }
    m_TextContentForOpenGL->Render(GL_TRIANGLES, true);
}

void Tilc::Gui::TMenu::Draw(float x, float y, bool CenterHorizontally, bool CenterVertically)
{
	int TexNumber = 0;
	SDL_Texture* Image = nullptr;
	SDL_FRect ImageRect;
    m_PosX = x;
    m_PosY = y;
	static SDL_FRect DestRect;
	SDL_Renderer* Renderer = Tilc::GameObject->m_Window->GetRenderer();

	DestRect = GetBoundingBox();
	if (m_MenuBackground)
	{
		DestRect.x = m_PosX - 18;
		DestRect.y = m_PosY - 20;
		DestRect.w += 36;
		DestRect.h += 40;
		SDL_SetTextureAlphaMod(m_MenuBackground, static_cast<int>((1.0f - m_Alpha / 100.0f) * 255));
		SDL_RenderTexture(Renderer, m_MenuBackground, NULL, &DestRect);
	}
	for (int i = 0; i < m_MenuItems.size(); i++)
	{
		if (i == m_SelectedIndex)
		{
            m_Font->SetColor(m_ColorSelected);
		}
		else
		{
            m_Font->SetColor(m_Color);
		}
        m_Font->DrawString(Renderer, m_MenuItems[i].m_Caption.c_str(), x, y);

		if (m_MenuItems[i].m_DrawIcon)
		{
			if (m_MenuItems[i].m_Available)
			{
				Image = m_MenuEnabledImage;
				ImageRect = m_MenuEnabledImageRect;
			}
			else
			{
				Image = m_MenuDisabledImage;
				ImageRect = m_MenuDisabledImageRect;
			}
			if (Image)
			{
				ImageRect.x = x - ImageRect.w - 8.0f;
				ImageRect.y = y - 4.0f; // -ImageRect.h / 2.0f;
				SDL_RenderTexture(Renderer, Image, NULL, &ImageRect);
			}
		}
		y += m_ItemHeight + m_VSpacing;
	}
}

int Tilc::Gui::TMenu::GetMenuItemIndexAtPoint(float x, float y)
{
    // if it is not OpenGL mode
    if (!m_TextContentForOpenGL)
    {
        if (x < m_PosX || x > m_PosX + m_MenuWidth || y < m_PosY || y > m_PosY + m_MenuHeight)
        {
            return -1;
        }

        float top = 0;
        for (int i = 0; i < m_MenuItems.size(); i++)
        {
            float bottom = top + m_ItemHeight + m_VSpacing;
            if (y >= m_PosY + top && y <= m_PosY + bottom)
            {
                return i;
            }
            top += m_ItemHeight + m_VSpacing;
        }
    }
    else
    {
        // if it is OpenGL mode
        Tilc::TVector2D<int> WindowSize = Tilc::GameObject->GetContext()->m_Window->GetWindowSize();
        float OpenGL_x = -1 + ((static_cast<float>(x) / WindowSize.x) * 2.0f);
        float OpenGL_y = 1 - ((static_cast<float>(y) / WindowSize.y) * 2.0f);
        float OpenGL_MenuWidth = (static_cast<float>(m_MenuWidth) / WindowSize.x) * 2.0f;
        float OpenGL_MenuHeight = (static_cast<float>(m_MenuHeight) / WindowSize.y) * 2.0f;
        if (OpenGL_x < m_PosX || OpenGL_x > m_PosX + OpenGL_MenuWidth || OpenGL_y > m_PosY || OpenGL_y < m_PosY - OpenGL_MenuHeight)
        {
            return -1;
        }

        float top = 0;
        float OpenGL_ItemHeight = (static_cast<float>(m_ItemHeight + m_VSpacing) / WindowSize.y) * 2.0f;
        for (int i = 0; i < m_MenuItems.size(); i++)
        {
            float bottom = top + OpenGL_ItemHeight;
            if (OpenGL_y <= m_PosY - top && OpenGL_y >= m_PosY - bottom)
            {
                return i;
            }
            top += OpenGL_ItemHeight;
        }
    }

	return -1;
}

SDL_FRect Tilc::Gui::TMenu::GetBoundingBox()
{
	float W, H, MaxW = 0;
	SDL_FRect r;
	r.x = m_PosX;
	r.y = m_PosY;
	r.w = static_cast<float>(m_MenuWidth);
	r.h = static_cast<float>(m_MenuHeight);

	if (m_MenuEnabledImage)
	{
		SDL_GetTextureSize(m_MenuEnabledImage, &W, &H);
        m_MenuEnabledImageRect.x = 0.0f;
        m_MenuEnabledImageRect.y = 0.0f;
        m_MenuEnabledImageRect.w = SDL_clamp(W, 60, 60);
        m_MenuEnabledImageRect.h = SDL_clamp(H, 60, 60);
		MaxW = W + 2 * 20;
	}
	if (m_MenuDisabledImage)
	{
		SDL_GetTextureSize(m_MenuDisabledImage, &W, &H);
        m_MenuDisabledImageRect.x = 0.0f;
        m_MenuDisabledImageRect.y = 0.0f;
        m_MenuDisabledImageRect.w = SDL_clamp(W, 60, 60);
        m_MenuDisabledImageRect.h = SDL_clamp(H, 60, 60);
		float CurMaxW = W + 2 * 20;
		if (MaxW < CurMaxW)
		{
			MaxW = CurMaxW;
		}
	}
	r.w += MaxW;

	return r;
}

bool Tilc::Gui::TMenu::OnMouseMove(const SDL_Event& event)
{
    int MenuItemIndexAtCursor = GetMenuItemIndexAtPoint(static_cast<float>(event.motion.x), static_cast<float>(event.motion.y));
    if (MenuItemIndexAtCursor >= 0 && MenuItemIndexAtCursor != GetSelectedIndex())
    {
        //Mix_PlayChannel(0, WavMenuSelected, 0);
        if (m_WavMenuSelected && Tilc::GameObject->m_State.m_EffectsOn)
        {
            m_WavMenuSelected->Play(Tilc::GameObject->m_State.m_EffectVolume, 0);
        }
        SetSelectedIndex(MenuItemIndexAtCursor);
        m_NeedRegenerateSurface = true;
        m_TextContentForOpenGL->m_NeedRegenerateBuffers = true;
        return true;
    }
    return false;
}

bool Tilc::Gui::TMenu::OnMouseButtonDown(const SDL_Event& Event)
{
	SDL_FRect r;
	SDL_FPoint p;
	p.x = Event.motion.x;
	p.y = Event.motion.y;
	r = GetBoundingBox();
	if (SDL_PointInRectFloat(&p, &r))
	{
		if (m_WavMenuActivated && Tilc::GameObject->m_State.m_EffectsOn)
		{
            m_WavMenuActivated->Play(Tilc::GameObject->m_State.m_EffectVolume, 0);
		}
		return GetSelectedIndex();
	}
	return false;
}

bool Tilc::Gui::TMenu::OnKeyDown(const SDL_Event& Event)
{
	switch (Event.key.key)
	{
	case SDLK_DOWN:
		//Mix_PlayChannel(0, WavMenuSelected, 0);
		if (m_WavMenuSelected && Tilc::GameObject->m_State.m_EffectsOn)
		{
            m_WavMenuSelected->Play(Tilc::GameObject->m_State.m_EffectVolume, 0);
		}
		SelectNextItem();
		return 1;
	case SDLK_UP:
		if (m_WavMenuSelected && Tilc::GameObject->m_State.m_EffectsOn)
		{
            m_WavMenuSelected->Play(Tilc::GameObject->m_State.m_EffectVolume, 0);
		}
		SelectPrevItem();
		return 1;
	case SDLK_RETURN:
		if (m_WavMenuActivated && Tilc::GameObject->m_State.m_EffectsOn)
		{
            m_WavMenuActivated->Play(Tilc::GameObject->m_State.m_EffectVolume, 0);
		}
		return 0;
	}
	return 0;
}
