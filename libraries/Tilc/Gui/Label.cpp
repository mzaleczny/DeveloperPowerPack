#include "Tilc/Gui/Label.h"
#include "Tilc/Gui/Font.h"
#include "Tilc/Gui/Theme.h"

Tilc::Gui::TLabel::TLabel(TGuiControl* parent, const Tilc::TExtString& name, const SDL_FRect& position, Tilc::TExtString text, bool autosize, int LineHeight)
    : Tilc::Gui::TGuiControl(parent, name, position, Tilc::Gui::EControlType::ECT_Label), m_LineHeight(LineHeight), m_AutoSize(autosize)
{    
    m_BorderType = Tilc::Gui::EControlBorderType::ECBT_None;
    SetText(text);
}

Tilc::Gui::TLabel::~TLabel()
{
}

void Tilc::Gui::TLabel::Draw()
{
    m_NeedUpdate = ENeedUpdate::ENU_None;

    Tilc::Gui::TFont* Font = Tilc::GameObject->GetFont(m_FontNameToUse);
    TTheme* m_Theme = Tilc::GameObject->GetContext()->m_Theme;
    SDL_Texture* OldRenderTarget{};
    float x = 0;
    float y = 0;
    SDL_Texture* TextureMap = m_Theme->GuiTextureMap1;
    SDL_FRect Rect = m_Position;

    if (m_PaddingLeft)
    {
        Rect.x += m_PaddingLeft;
        Rect.w -= m_PaddingLeft;
    }
    if (m_PaddingRight)
    {
        Rect.w -= m_PaddingRight;
    }
    if (m_PaddingTop)
    {
        Rect.y += m_PaddingTop;
        Rect.h -= m_PaddingTop;
    }
    if (m_PaddingBottom)
    {
        Rect.h -= m_PaddingBottom;
    }

    if (m_Canvas)
    {
        OldRenderTarget = SDL_GetRenderTarget(Renderer);
        SDL_SetRenderTarget(Renderer, m_Canvas);
    }

    SDL_Rect ClipRect, OldClipRect;
    ClipRect.x = m_Position.x;
    ClipRect.y = m_Position.y;
    ClipRect.w = m_Position.w;
    ClipRect.h = m_Position.h;

    SDL_GetRenderClipRect(Renderer, &OldClipRect);
    SDL_SetRenderClipRect(Renderer, &ClipRect);
    if (!m_IsTransparentBg)
    {
        SDL_SetRenderDrawColor(Renderer, m_BgColor.r, m_BgColor.g, m_BgColor.b, m_BgColor.a);
        SDL_RenderFillRect(Renderer, &m_Position);
    }
    if (m_BorderType == Tilc::Gui::EControlBorderType::ECBT_Solid)
    {
        SDL_SetRenderDrawColor(Renderer, m_BorderColor.r, m_BorderColor.g, m_BorderColor.b, m_BorderColor.a);
        SDL_RenderRect(Renderer, &m_Position);
    }
    if (Font)
    {
        x = Rect.x;
        y = Rect.y;
        Font->SetColor(m_Color);
        Font->m_WrapTo = (m_Wrap) ? Rect.w : 0;
        Font->DrawString(Renderer, m_Text.c_str(), x, y);
    }
    if (OldClipRect.w == 0 || OldClipRect.h == 0)
    {
        SDL_SetRenderClipRect(Renderer, nullptr);
    }
    else
    {
        SDL_SetRenderClipRect(Renderer, &OldClipRect);
    }

    if (m_Canvas)
    {
        SDL_SetRenderTarget(Renderer, OldRenderTarget);
    }
}

void Tilc::Gui::TLabel::ResizeLabelIfAutoSize()
{
    if (m_AutoSize)
    {
        Tilc::Gui::TFont* Font = Tilc::GameObject->GetFont(m_FontNameToUse);
        if (Font)
        {
            int w, h;
            Font->GetTextSize(m_Text.c_str(), w, h);
            w += m_PaddingLeft + m_PaddingRight;
            h += m_PaddingTop + m_PaddingBottom;
            SetSize(w, h);
        }
    }
}

void Tilc::Gui::TLabel::SetPadding(unsigned int padding)
{
    m_PaddingLeft = padding;
    m_PaddingRight = padding;
    m_PaddingTop = padding;
    m_PaddingBottom = padding;
    ResizeLabelIfAutoSize();
    Invalidate();
}

void Tilc::Gui::TLabel::SetPadding(unsigned int paddingLeftRight, unsigned int paddingTopBottom)
{
    m_PaddingLeft = paddingLeftRight;
    m_PaddingRight = paddingLeftRight;
    m_PaddingTop = paddingTopBottom;
    m_PaddingBottom = paddingTopBottom;
    ResizeLabelIfAutoSize();
    Invalidate();
}
