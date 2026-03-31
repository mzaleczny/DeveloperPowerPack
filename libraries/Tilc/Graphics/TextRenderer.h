#pragma once

#include "Tilc/DllGlobals.h"
#include <Tilc/Utils/ExtString.h>
#include <Tilc/Graphics/Mesh.h>
#include "Tilc/Utils/Matrix.h"
#include <unordered_map>
#include <SDL3/SDL.h>

namespace Tilc::Gui
{
    class TFont;
}

namespace Tilc::Graphics
{
    const constexpr int TextTextureWidth = 2048;
    const constexpr int TextTextureHeight = 2048;

    struct DECLSPEC FTextTextureInfo
    {
        SDL_FRect m_Rect;
        SDL_Color m_Color;
        SDL_Color m_ShadowColor;
        Tilc::Gui::TFont* m_Font = nullptr;
        float m_Size;
        float m_OnScreenX;
        float m_OnScreenY;
        bool m_HalfFloat;
        int m_ShadowOffsetX;
        int m_ShadowOffsetY;
        bool m_DrawShadow;
    };

    using TFontNameToFontInfoMap = std::unordered_map<Tilc::TExtString, FTextTextureInfo>;
    using TTextToFontsMap = std::unordered_map<Tilc::TExtString, TFontNameToFontInfoMap>;

	class DECLSPEC TTextRenderer : public TMesh
	{
	public:
        TTextRenderer(const Tilc::TExtString& TextureResourceName = "TextTexture", int SurfaceWidth = TextTextureWidth, int SurfaceHeight = TextTextureHeight);
        TTextRenderer(Tilc::Graphics::OpenGL::TPipeline* Pipeline, const Tilc::TExtString& TextureResourceName = "TextTexture", int SurfaceWidth = TextTextureWidth, int SurfaceHeight = TextTextureHeight);
        virtual ~TTextRenderer();

        virtual void Render(GLenum DrawType, bool UseEBO = true, int InstancesCount = 0) override;
        virtual void UpdateVertexArraysAndVAO(bool HalfFloat) override;
        FTextTextureInfo* AddText(const Tilc::TExtString& Text, const Tilc::TExtString& FontName);
        void Clear() { m_TextTextureInfo.clear(); }
        TFontNameToFontInfoMap* Find(const Tilc::TExtString& Text);
        FTextTextureInfo* Find(const Tilc::TExtString& Text, const Tilc::TExtString& FontName);
        // Generates surface with outputted all stored texts
        void GenerateTextSurface();
        void PushTextForRender(float OnScreenX, float OnScreenY, const Tilc::TExtString& Text, const Tilc::TExtString& FontName, SDL_Color Color, bool DrawShadow = false, int ShadowOffsetX = 2, int ShadowOffsetY = 2, SDL_Color ShadowColor = { 33, 33, 33, 255 });
        void PushTextForRender16(float OnScreenX, float OnScreenY, const Tilc::TExtString& Text, const Tilc::TExtString& FontName, SDL_Color Color, bool DrawShadow = false, int ShadowOffsetX = 2, int ShadowOffsetY = 2, SDL_Color ShadowColor = { 33, 33, 33, 255 });

        SDL_Surface* m_Surface = nullptr;
        int m_SurfaceWidth = TextTextureWidth;
        int m_SurfaceHeight = TextTextureHeight;
        Tilc::TExtString m_TextureResourceName;

        // Text => [FontName => FontInfo]
        TTextToFontsMap m_TextTextureInfo;
    private:
        bool m_DestroyPipeline = false;
	};
}
