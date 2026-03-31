#include "Tilc/Graphics/Model.h"
#include "Tilc/Graphics/Camera.h"
#include "Tilc/Gui/Font.h"
#include "Tilc/Game.h"
#include "Tilc/Graphics/TextRenderer.h"
#include "Tilc/Resources/TextureManager.h"
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3_image/SDL_image.h>

Tilc::Graphics::TTextRenderer::TTextRenderer(const Tilc::TExtString& TextureResourceName, int SurfaceWidth, int SurfaceHeight)
    : TMesh(nullptr), m_DestroyPipeline(true), m_TextureResourceName(TextureResourceName), m_SurfaceWidth(SurfaceWidth), m_SurfaceHeight(SurfaceHeight)
{
    m_NeedRegenerateBuffers = true;
    m_Surface = SDL_CreateSurface(m_SurfaceWidth, m_SurfaceHeight, SDL_PIXELFORMAT_RGBA32);

    m_Pipeline = new Tilc::Graphics::OpenGL::TPipeline(
        R"(
        #version 450 core
        #extension GL_ARB_separate_shader_objects: enable

        layout (location = 0) in vec2 VertexPos;
        layout (location = 1) in vec2 VertexTexCoord;

        out vec2 VertTexCoord;

        out gl_PerVertex
        {
            vec4 gl_Position;
            float gl_PointSize;
            float gl_ClipDistance[];
        };

        layout (location = 0) uniform mat4 Model;

        void main()
        {
	        gl_Position = Model * vec4(VertexPos, 0.0f, 1.0f);
            VertTexCoord = VertexTexCoord;
        })",


        R"(
        #version 450 core
        in vec2 VertTexCoord;
        out vec4 FragColor;

        layout (binding = 0) uniform sampler2D tex;
        layout (location = 0) uniform float Alpha;

        void main()
        {
            if (Alpha > 0.99f)
            {
		        FragColor = texture(tex, VertTexCoord).rgba;
            }
            else
            {
		        FragColor = vec4(texture(tex, VertTexCoord).rgb, Alpha);
            }
        })",


        ""
    );
}

Tilc::Graphics::TTextRenderer::TTextRenderer(Tilc::Graphics::OpenGL::TPipeline* Pipeline, const Tilc::TExtString& TextureResourceName, int SurfaceWidth, int SurfaceHeight)
    : TMesh(Pipeline), m_DestroyPipeline(false), m_TextureResourceName(TextureResourceName), m_SurfaceWidth(SurfaceWidth), m_SurfaceHeight(SurfaceHeight)
{
    m_NeedRegenerateBuffers = true;
    m_Surface = SDL_CreateSurface(m_SurfaceWidth, m_SurfaceHeight, SDL_PIXELFORMAT_RGBA32);
}

Tilc::Graphics::TTextRenderer::~TTextRenderer()
{
    if (Tilc::GameObject->GetContext()->m_TextureManager)
    {
        while (Tilc::GameObject->GetContext()->m_TextureManager->ReleaseResource(m_TextureResourceName))
        {
        }
    }

    if (m_DestroyPipeline && m_Pipeline)
    {
        delete m_Pipeline;
        m_Pipeline = nullptr;
    }

    m_TextTextureInfo.clear();
    if (m_Surface)
    {
        SDL_DestroySurface(m_Surface);
        m_Surface = nullptr;
    }
}

void Tilc::Graphics::TTextRenderer::Render(GLenum DrawType, bool UseEBO, int InstancesCount)
{
    GLint IsBlendOn, BlendSFactor, BlendDFactor;
    glGetIntegerv(GL_BLEND, &IsBlendOn);
    if (IsBlendOn)
    {
        glGetIntegerv(GL_BLEND_SRC_RGB, &BlendSFactor);
        glGetIntegerv(GL_BLEND_DST_RGB, &BlendDFactor);
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    Tilc::TMatrix4<float> ModelMatrix = Tilc::TMatrix4<float>(1.0f);
    Tilc::TMatrix4<float> ViewMatrix = Tilc::LookAtRH(
        Tilc::TVector3D<float>(0.0f, 0.0f, 6.0f),
        Tilc::TVector3D<float>(0.0f, 0.0f, 0.0f),
        Tilc::TVector3D<float>(0.0f, 1.0f, 0.0f)
    );
    Tilc::TMatrix4<float> FontProjectionMatrix = Tilc::Ortho(0, -1.0, 1.0f, -1.0f, 1.0f, 0.1f, 100.0f);
    Tilc::TMatrix4<float> MVPMatrix = FontProjectionMatrix * ViewMatrix * ModelMatrix;
    m_Pipeline->Bind();
    m_Pipeline->SetMat4(m_Pipeline->m_VertexShader, "Model", MVPMatrix.Elems);
    m_Pipeline->SetFloat(m_Pipeline->m_FragmentShader, "Alpha", 1.0f);
    TMesh::Render(DrawType, UseEBO, InstancesCount);

    if (!IsBlendOn)
    {
        glDisable(GL_BLEND);
    }
    else
    {
        glBlendFunc(BlendSFactor, BlendDFactor);
    }
}

void Tilc::Graphics::TTextRenderer::UpdateVertexArraysAndVAO(bool HalfFloat)
{
    Tilc::TVector2D<int> WindowSize = Tilc::GameObject->GetContext()->m_Window->GetWindowSize();

    GenerateTextSurface();

    int TextTextureWidth = m_SurfaceWidth;
    int TextTextureHeight = m_SurfaceHeight;
    float x = 0.0f;
    float y = 0.0f;
    auto ItrText = m_TextTextureInfo.begin();
    while (ItrText != m_TextTextureInfo.end())
    {
        auto ItrFont = ItrText->second.begin();
        while (ItrFont != ItrText->second.end())
        {
            const Tilc::TExtString& FontName = ItrFont->first;
            FTextTextureInfo& Info = ItrFont->second;

            const SDL_FRect& Rect = Info.m_Rect;
            if (Info.m_HalfFloat)
            {
                int StartVertexIndex = static_cast<int>(m_VertexData16.size()) / 4;
                PushTexturedShape16({
                    Info.m_OnScreenX, Info.m_OnScreenY, Rect.x / TextTextureWidth, Rect.y / TextTextureHeight,
                    Info.m_OnScreenX, Info.m_OnScreenY - (Rect.h / WindowSize.y) * 2.0f, Rect.x, (Rect.y + Rect.h) / TextTextureHeight,
                    Info.m_OnScreenX + (Rect.w / WindowSize.x) * 2.0f, Info.m_OnScreenY - (Rect.h / WindowSize.y) * 2.0f, (Rect.x + Rect.w) / TextTextureWidth, (Rect.y + Rect.h) / TextTextureHeight,
                    Info.m_OnScreenX + (Rect.w / WindowSize.x) * 2.0f, Info.m_OnScreenY, (Rect.x + Rect.w) / TextTextureWidth, Rect.y / TextTextureHeight,
                    }, {
                        // Counter-Clockwise direction
                        0, 1, 2,
                        2, 3, 0,
                    }, StartVertexIndex);
            }
            else
            {
                int StartVertexIndex = static_cast<int>(m_VertexData.size()) / 4;
                PushTexturedShape({
                    Info.m_OnScreenX, Info.m_OnScreenY, Rect.x / TextTextureWidth, Rect.y / TextTextureHeight,
                    Info.m_OnScreenX, Info.m_OnScreenY - (Rect.h / WindowSize.y) * 2.0f, Rect.x, (Rect.y + Rect.h) / TextTextureHeight,
                    Info.m_OnScreenX + (Rect.w / WindowSize.x) * 2.0f, Info.m_OnScreenY - (Rect.h / WindowSize.y) * 2.0f, (Rect.x + Rect.w) / TextTextureWidth, (Rect.y + Rect.h) / TextTextureHeight,
                    Info.m_OnScreenX + (Rect.w / WindowSize.x) * 2.0f, Info.m_OnScreenY, (Rect.x + Rect.w) / TextTextureWidth, Rect.y / TextTextureHeight,
                    }, {
                        // Counter-Clockwise direction
                        0, 1, 2,
                        2, 3, 0,
                    }, StartVertexIndex);
            }
            ++ItrFont;
        }
        ++ItrText;
    }

    if (m_VertexData16.size() > 0 || m_VertexData.size() > 0)
    {
        m_Textures.push_back({ 0, Tilc::GameObject->GetContext()->m_TextureManager->GetResource(m_TextureResourceName)->AsGLTexture() });
    }

    DeleteVAOAndBuffers();
    if (HalfFloat && m_VertexData16.size() > 0)
    {
        CreateVAOAndBuffers<Tilc::float16, GLubyte>(m_VertexData16.size() / 4);
    }
    else if (m_VertexData.size() > 0)
    {
        CreateVAOAndBuffers<float, GLuint>(m_VertexData.size() / 4);
    }
    m_NeedRegenerateBuffers = false;
}

Tilc::Graphics::FTextTextureInfo* Tilc::Graphics::TTextRenderer::AddText(const Tilc::TExtString& Text, const Tilc::TExtString& FontName)
{
    // If text not found in map
    TFontNameToFontInfoMap* FontNameToFontInfo = Find(Text);
    if (!FontNameToFontInfo)
    {
        // then add it along with fontname info
        FTextTextureInfo Info;
        Info.m_Font = Tilc::GameObject->GetFont(FontName.c_str());
        if (Info.m_Font)
        {
            Info.m_Size = Info.m_Font->GetSize();
            TFontNameToFontInfoMap FontToInfo;
            FontToInfo.emplace(FontName, Info);
            m_TextTextureInfo.emplace(Text, FontToInfo);
            m_NeedRegenerateBuffers = true;
            if (m_Textures.size() > 0)
            {
                m_Textures.clear();
            }
            return &m_TextTextureInfo[Text][FontName];
        }
    }
    else
    {
        // otherwise add only fontname info
        FTextTextureInfo Info;
        Info.m_Font = Tilc::GameObject->GetFont(FontName.c_str());
        if (Info.m_Font)
        {
            Info.m_Size = Info.m_Font->GetSize();
            FontNameToFontInfo->emplace(FontName, Info);
            m_NeedRegenerateBuffers = true;
            if (m_Textures.size() > 0)
            {
                m_Textures.clear();
            }
            return &(*FontNameToFontInfo)[FontName];
        }
    }
    return nullptr;
}

Tilc::Graphics::TFontNameToFontInfoMap* Tilc::Graphics::TTextRenderer::Find(const Tilc::TExtString& Text)
{
    auto ItrText  = m_TextTextureInfo.find(Text);
    if (ItrText != m_TextTextureInfo.end())
    {
        return &ItrText->second;
    }
    return nullptr;
}

Tilc::Graphics::FTextTextureInfo* Tilc::Graphics::TTextRenderer::Find(const Tilc::TExtString& Text, const Tilc::TExtString& FontName)
{
    auto ItrText  = m_TextTextureInfo.find(Text);
    if (ItrText != m_TextTextureInfo.end())
    {
        auto ItrInfo = ItrText->second.find(FontName);
        if (ItrInfo != ItrText->second.end())
        {
            return &ItrInfo->second;
        }
    }
    return nullptr;
}

void Tilc::Graphics::TTextRenderer::GenerateTextSurface()
{
    if (!m_Surface)
    {
        return;
    }

    SDL_ClearSurface(m_Surface, 0.0f, 0.0f, 0.0f, 0.0f);
    SDL_Color Color;

    float x = 0.0f;
    float y = 0.0f;
    auto ItrText = m_TextTextureInfo.begin();
    while (ItrText != m_TextTextureInfo.end())
    {
        auto ItrFont = ItrText->second.begin();
        while (ItrFont != ItrText->second.end())
        {
            const Tilc::TExtString& FontName = ItrFont->first;
            FTextTextureInfo& Info = ItrFont->second;
            int w, h;
            Info.m_Font->GetTextSize(ItrText->first.c_str(), w, h);
            if (y + h > TextTextureHeight)
            {
                break;
            }
            Info.m_Rect.x = x;
            Info.m_Rect.y = y;
            Info.m_Rect.w = w;
            Info.m_Rect.h = h;

            Color = Info.m_Font->GetColor();
            if (Info.m_DrawShadow)
            {
                Info.m_Font->SetColor(Info.m_ShadowColor);
                Info.m_Font->DrawString(m_Surface, ItrText->first.c_str(), x + Info.m_ShadowOffsetX, y + Info.m_ShadowOffsetY);
            }
            Info.m_Font->SetColor(Info.m_Color);
            Info.m_Font->DrawString(m_Surface, ItrText->first.c_str(), x, y);
            Info.m_Font->SetColor(Color);

            y += h;

            ++ItrFont;
        }
        ++ItrText;
    }
    //IMG_SavePNG(m_Surface, "Test.png");
    Tilc::Resources::TResourceTexture* Resource = Tilc::GameObject->GetContext()->m_TextureManager->CreateGLTextureResourceFromSDLSurface(m_Surface);
    if (Resource)
    {
        while (Tilc::GameObject->GetContext()->m_TextureManager->ReleaseResource(m_TextureResourceName)) {}
        Tilc::GameObject->GetContext()->m_TextureManager->AddResource(m_TextureResourceName, Resource);
    }
}

void Tilc::Graphics::TTextRenderer::PushTextForRender(float OnScreenX, float OnScreenY, const Tilc::TExtString& Text, const Tilc::TExtString& FontName, SDL_Color Color, bool DrawShadow, int ShadowOffsetX, int ShadowOffsetY, SDL_Color ShadowColor)
{
    FTextTextureInfo* Info = Find(Text, FontName);
    if (!Info)
    {
        Info = AddText(Text, FontName);
    }
    if (Info)
    {
        Info->m_HalfFloat = true;
        Info->m_OnScreenX = OnScreenX;
        Info->m_OnScreenY = OnScreenY;
        Info->m_Color = Color;
        Info->m_DrawShadow = DrawShadow;
        Info->m_ShadowColor = ShadowColor;
        Info->m_ShadowOffsetX = ShadowOffsetX;
        Info->m_ShadowOffsetY = ShadowOffsetY;
    }
}

void Tilc::Graphics::TTextRenderer::PushTextForRender16(float OnScreenX, float OnScreenY, const Tilc::TExtString& Text, const Tilc::TExtString& FontName, SDL_Color Color, bool DrawShadow, int ShadowOffsetX, int ShadowOffsetY, SDL_Color ShadowColor)
{
    FTextTextureInfo* Info = Find(Text, FontName);
    if (!Info)
    {
        Info = AddText(Text, FontName);
    }
    if (Info)
    {
        Info->m_HalfFloat = true;
        Info->m_OnScreenX = OnScreenX;
        Info->m_OnScreenY = OnScreenY;
        Info->m_Color = Color;
        Info->m_DrawShadow = DrawShadow;
        Info->m_ShadowColor = ShadowColor;
        Info->m_ShadowOffsetX = ShadowOffsetX;
        Info->m_ShadowOffsetY = ShadowOffsetY;
    }
}
