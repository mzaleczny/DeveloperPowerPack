#include "Tilc/Gui/Font.h"
#include "Tilc/Utils/ExtString.h"
#include "Tilc/Data/DataDbResources.h"
#include "SDL3/SDL.h"
#include "SDL3_ttf/SDL_ttf.h"

Tilc::Gui::TTextSizeCache Tilc::Gui::TFont::TextSizesCache;

Tilc::Gui::TFont::TFont(const char* FontResourceName, float size, bool FromFile)
{
	size_t BufferSize;
	Tilc::TExtString ResOrigPath;

	m_Size = size;
	m_ReleaseFont = true;
    m_FontName = FontResourceName;
    m_FromFile = FromFile;
    if (FromFile)
	{
        m_FontFilePath = FontResourceName;
		m_Font = TTF_OpenFont(FontResourceName, size);
        if (!m_Font)
        {
            SDL_Log("Couldn't open font %s: %s", FontResourceName, SDL_GetError());
        }
		m_FontResourceData = nullptr;
	}
	else
	{
		Tilc::Data::Data->GetResourceByName(Tilc::Data::Data->GetDbFname(), FontResourceName, ResOrigPath, &m_FontResourceData, &BufferSize, 0);
		if (m_FontResourceData)
		{
            m_FontResourceDataBufferSize = BufferSize;
			m_Font = TTF_OpenFontIO(SDL_IOFromConstMem(m_FontResourceData, BufferSize), true, size);
		}
	}

    if (m_Font)
    {
        m_Engine = TTF_CreateSurfaceTextEngine();
        TTF_SetFontStyle(m_Font, TTF_STYLE_NORMAL);
        TTF_SetFontOutline(m_Font, 0);
        TTF_SetFontHinting(m_Font, TTF_HINTING_NORMAL);
    }
}

Tilc::Gui::TFont::TFont(TTF_Font* ExistingFont)
{
	m_Font = ExistingFont;
	m_ReleaseFont = false;
	if (m_Font)
	{
        m_Engine = TTF_CreateSurfaceTextEngine();
        m_Size = TTF_GetFontSize(m_Font);
	}
	m_FontResourceData = nullptr;
}

Tilc::Gui::TFont::~TFont()
{
    if (m_FT)
    {
        if (m_HBFont)
        {
            hb_font_destroy(m_HBFont);
            m_HBFont = nullptr;
        }
        if (m_Face)
        {
            FT_Done_Face(m_Face);
        }
        FT_Done_FreeType(m_FT);
    }

    if (m_Engine)
    {
        TTF_DestroySurfaceTextEngine(m_Engine);
    }
	if (m_FontResourceData)
	{
		delete[] m_FontResourceData;
		m_FontResourceData = nullptr;
	}
	if (m_ReleaseFont && m_Font)
	{
		TTF_CloseFont(m_Font);
		m_Font = nullptr;
	}
}

void Tilc::Gui::TFont::DrawString(SDL_Renderer* Renderer, const char* String, float x, float y)
{
    SDL_Surface* TextSurface{};
    if (m_WrapTo > 0)
    {
        TextSurface = TTF_RenderText_Blended_Wrapped(m_Font, String, 0, m_Color, m_WrapTo);
    }
    else
    {
        TextSurface = TTF_RenderText_Blended(m_Font, String, 0, m_Color);
    }
    if (TextSurface)
	{
		SDL_Texture* TextTexture = SDL_CreateTextureFromSurface(Renderer, TextSurface);
		if (TextTexture != nullptr)
		{
			SDL_FRect destRect;
			destRect.x = x;
			destRect.y = y;
			destRect.w = static_cast<float>(TextSurface->w);
			destRect.h = static_cast<float>(TextSurface->h);

            SDL_RenderTexture(Renderer, TextTexture, nullptr, &destRect);
			SDL_DestroyTexture(TextTexture);
		}
		SDL_DestroySurface(TextSurface);
	}
}

void Tilc::Gui::TFont::DrawString(SDL_Renderer* Renderer, const char* String, SDL_FRect* DestRect, int Align)
{
    SDL_Surface* TextSurface{};
    if (m_WrapTo > 0)
    {
        TextSurface = TTF_RenderText_Blended_Wrapped(m_Font, String, 0, m_Color, m_WrapTo);
    }
    else
    {
        TextSurface = TTF_RenderText_Blended(m_Font, String, 0, m_Color);
    }
    if (TextSurface)
	{
		SDL_Texture* TextTexture = SDL_CreateTextureFromSurface(Renderer, TextSurface);
		if (TextTexture != nullptr)
		{
			SDL_FRect Rect;
			Rect.x = DestRect->x;
			Rect.y = DestRect->y;
			Rect.w = static_cast<float>(TextSurface->w);
			Rect.h = static_cast<float>(TextSurface->h);

            if (Align & Tilc::Gui::Align_Left)
            {
                Rect.x = DestRect->x;
            }
            else if (Align & Tilc::Gui::Align_Right)
            {
                Rect.x = DestRect->x + DestRect->w - TextSurface->w;
            }
            else if (Align & Tilc::Gui::Align_CenterHorizontal)
            {
                Rect.x = DestRect->x + (DestRect->w - TextSurface->w) / 2.0f;
            }

            if (Align & Tilc::Gui::Align_Top)
            {
                Rect.y = DestRect->y;
            }
            else if (Align & Tilc::Gui::Align_Bottom)
            {
                Rect.y = DestRect->y + DestRect->h - TextSurface->h;
            }
            else if (Align & Tilc::Gui::Align_CenterVertical)
            {
                Rect.y = DestRect->y + (DestRect->h - TextSurface->h) / 2.0f;
            }

            SDL_RenderTexture(Renderer, TextTexture, nullptr, &Rect);
			SDL_DestroyTexture(TextTexture);
		}
		SDL_DestroySurface(TextSurface);
	}
}

void Tilc::Gui::TFont::DrawString(SDL_Surface* Surface, const char* String, float x, float y)
{
    SDL_Surface* TextSurface{};
    if (m_WrapTo > 0)
    {
        TextSurface = TTF_RenderText_Blended_Wrapped(m_Font, String, 0, m_Color, m_WrapTo);
    }
    else
    {
        TextSurface = TTF_RenderText_Blended(m_Font, String, 0, m_Color);
    }
    if (TextSurface)
	{
		SDL_Rect Rect{ (int)x, (int)y, TextSurface->w, TextSurface->h };
		SDL_BlitSurface(TextSurface, nullptr, Surface, &Rect);
		SDL_DestroySurface(TextSurface);
	}
}

void Tilc::Gui::TFont::DrawString(SDL_Surface* Surface, const char* String, SDL_FRect* DestRect, int Align)
{
    SDL_Surface* TextSurface{};
    if (m_WrapTo > 0)
    {
        TextSurface = TTF_RenderText_Blended_Wrapped(m_Font, String, 0, m_Color, m_WrapTo);
    }
    else
    {
        TextSurface = TTF_RenderText_Blended(m_Font, String, 0, m_Color);
    }
    if (TextSurface)
	{
		SDL_Rect Rect{ static_cast<int>(DestRect->x), static_cast<int>(DestRect->y), TextSurface->w, TextSurface->h };

        if (Align & Tilc::Gui::Align_Left)
        {
            Rect.x = DestRect->x;
        }
        else if (Align & Tilc::Gui::Align_Right)
        {
            Rect.x = DestRect->x + DestRect->w - TextSurface->w;
        }
        else if (Align & Tilc::Gui::Align_CenterHorizontal)
        {
            Rect.x = DestRect->x + (DestRect->w - TextSurface->w) / 2.0f;
        }

        if (Align & Tilc::Gui::Align_Top)
        {
            Rect.y = DestRect->y;
        }
        else if (Align & Tilc::Gui::Align_Bottom)
        {
            Rect.y = DestRect->y + DestRect->h - TextSurface->h;
        }
        else if (Align & Tilc::Gui::Align_CenterVertical)
        {
            Rect.y = DestRect->y + (DestRect->h - TextSurface->h) / 2.0f;
        }

		SDL_BlitSurface(TextSurface, nullptr, Surface, &Rect);
		SDL_DestroySurface(TextSurface);
	}
}

void Tilc::Gui::TFont::GetTextSize(const char* String, int& Width, int& Height)
{
	Width = 0;
	Height = 0;
    std::string s = String;

    if (TextSizesCache.find(m_Size) != TextSizesCache.end())
    {
        if (TextSizesCache[m_Size].find(s) != TextSizesCache[m_Size].end())
        {
            auto [w, h] = TextSizesCache[m_Size][s];
            Width = w;
            Height = h;
            return;
        }
    }

	if (TTF_Text* Text = TTF_CreateText(m_Engine, m_Font, String, 0))
	{
		TTF_GetTextSize(Text, &Width, &Height);
        TextSizesCache[m_Size][s] = TCachedSize(Width, Height);
		TTF_DestroyText(Text);
	}
}

void Tilc::Gui::TFont::GetXCoordForCenteredText(const char* String, float ContainerWidth, float& x)
{
	x = 0.0f;

	int Width = 0, Height = 0;
    std::string s = String;

    if (TextSizesCache.find(m_Size) != TextSizesCache.end())
    {
        if (TextSizesCache[m_Size].find(s) != TextSizesCache[m_Size].end())
        {
            auto [Width, Height] = TextSizesCache[m_Size][s];
            x = (ContainerWidth - Width) / 2.0f;
            return;
        }
    }

	if (TTF_Text* Text = TTF_CreateText(m_Engine, m_Font, String, 0))
	{
		TTF_GetTextSize(Text, &Width, &Height);
        TextSizesCache[m_Size][s] = TCachedSize(Width, Height);
		x = (ContainerWidth - Width) / 2.0f;
		TTF_DestroyText(Text);
	}
}

void Tilc::Gui::TFont::GetGetRectForCenteredText(const char* String, float ContainerWidth, SDL_FRect& r)
{
	memset(reinterpret_cast<void*>(&r), 0, sizeof(r));


	int Width = 0, Height = 0;
    std::string s = String;

    if (TextSizesCache.find(m_Size) != TextSizesCache.end())
    {
        if (TextSizesCache[m_Size].find(s) != TextSizesCache[m_Size].end())
        {
            auto [Width, Height] = TextSizesCache[m_Size][s];
            r.x = (ContainerWidth - Width) / 2.0f;
            r.y = 0;
            r.w = static_cast<float>(Width);
            r.h = static_cast<float>(Height);
            return;
        }
    }

	if (TTF_Text* Text = TTF_CreateText(m_Engine, m_Font, String, 0))
	{
		TTF_GetTextSize(Text, &Width, &Height);
        TextSizesCache[m_Size][s] = TCachedSize(Width, Height);

		r.x = (ContainerWidth - Width) / 2.0f;
		r.y = 0;
		r.w = static_cast<float>(Width);
		r.h = static_cast<float>(Height);

		TTF_DestroyText(Text);
	}
}

FT_Face& Tilc::Gui::TFont::GetFreeTypeFace()
{
    if (!m_FT && !m_Face)
    {
        FT_Init_FreeType(&m_FT);
        if (m_FromFile)
        {
            FT_New_Face(m_FT, m_FontFilePath.c_str(), 0, &m_Face);
        }
        else
        {
            FT_New_Memory_Face(m_FT, reinterpret_cast<FT_Byte*>(m_FontResourceData), m_FontResourceDataBufferSize, 0, &m_Face);
        }

        if (m_Face)
        {
            m_HBFont = hb_ft_font_create(m_Face, nullptr);
            FT_Set_Pixel_Sizes(m_Face, m_Size, m_Size);
            hb_ft_font_set_funcs(m_HBFont);
        }
    }
    return m_Face;
}
