#pragma once

#include "Tilc/DllGlobals.h"
#include "Tilc/Utils/ExtString.h"
#include "SDL3/SDL.h"
#include "SDL3_ttf/SDL_ttf.h"
#include <hb.h>
#include <hb-ft.h>
#include <unordered_map>

struct TTF_Font;
struct SDL_Renderer;

namespace Tilc {
    namespace Gui {

        constexpr const int Align_Left = 1;
        constexpr const int Align_Right = 2;
        constexpr const int Align_CenterHorizontal = 4;
        constexpr const int Align_Top = 8;
        constexpr const int Align_Bottom = 16;
        constexpr const int Align_CenterVertical = 32;

        using TCachedSize = std::pair<int, int>;
        using TCachedTextSize = std::unordered_map<std::string, TCachedSize>;
        using TTextSizeCache = std::unordered_map<float, TCachedTextSize>;

        class DECLSPEC TFont
        {
        public:
            TFont(const char* FontResourceName, float size, bool FromFile);
            TFont(TTF_Font* ExistingFont);
            ~TFont();
            float GetSize() const { return m_Size; }
            void DrawString(SDL_Renderer* Renderer, const char* String, float x, float y);
            void DrawString(SDL_Renderer* Renderer, const char* String, SDL_FRect* DestRect, int Align = Align_Left);
            void DrawString(SDL_Surface* Surface, const char* String, float x, float y);
            void DrawString(SDL_Surface* Surface, const char* String, SDL_FRect* DestRect, int Align = Align_Top);
            void GetTextSize(const char* String, int& Width, int& Height);
            inline void SetColor(const SDL_Color& Color) { this->m_Color = Color; }
            void GetXCoordForCenteredText(const char* String, float ContainerWidth, float& x);
            void GetGetRectForCenteredText(const char* String, float ContainerWidth, SDL_FRect& r);
            TTF_Font* GetTTFFont() { return m_Font; }
            SDL_Color GetColor() const { return m_Color; }
            FT_Face& GetFreeTypeFace();
            hb_font_t* GetHBFont() { return m_HBFont; }

            TTF_TextEngine* m_Engine{};
            TTF_Font* m_Font = nullptr;
            Tilc::TExtString m_FontName;
            Tilc::TExtString m_FontFilePath;
            size_t m_FontResourceDataBufferSize{};
            char* m_FontResourceData = nullptr;
            SDL_Color m_Color = { 255, 255, 255, 0 };
            SDL_Color m_BgColor = { 80, 80, 80, 255 };
            float m_Size{};
            bool m_ReleaseFont = true;
            bool m_FromFile{};
            // Size to which wrap text. If set to 0 or negative then no wrapping is performed.
            int m_WrapTo{};
        private:
            FT_Library m_FT{};
            FT_Face m_Face{};
            // HarfBuzz
            hb_font_t* m_HBFont{};
            static TTextSizeCache TextSizesCache;
        };

    }
}
