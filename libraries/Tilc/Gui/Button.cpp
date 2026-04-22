#include "Tilc/Gui/Button.h"
#include "Tilc/Gui/Theme.h"
#include "Tilc/Gui/Font.h"
#include "Tilc/Gui/StyledWindow.h"
#include "Tilc/Game.h"

Tilc::Gui::TButton::TButton(TGuiControl* parent, const Tilc::TExtString& name, const SDL_FRect& position, const Tilc::TExtString& text)
    : Tilc::Gui::TGuiControl(parent, name, position, Tilc::Gui::EControlType::ECT_Button)
{
    CommonInit(text);
}

void Tilc::Gui::TButton::CommonInit(const Tilc::TExtString& text)
{
    m_State = CONTROL_STATE_NORMAL;
    Tilc::Gui::TTheme* t = Tilc::GameObject->GetContext()->m_Theme;
    int min_width = t->button_left_rc.w + t->button_right_rc.w + 1;
    if (m_Position.w < min_width)
    {
        m_Position.w = min_width;
    }
    SetText(text);

    m_TabStop = true;
}

void Tilc::Gui::TButton::SetText(Tilc::TExtString text, bool redraw)
{
    Tilc::Gui::TGuiControl::SetText(text);
    CleanupText();
}

void Tilc::Gui::TButton::CleanupText()
{
    m_Text.RemoveCharsWithCodeLessThan(32);
}

void Tilc::Gui::TButton::Draw()
{
    TTheme* t = Tilc::GameObject->GetContext()->m_Theme;
    SDL_Texture* OldRenderTarget{ nullptr };

    if (m_Canvas)
    {
        OldRenderTarget = SDL_GetRenderTarget(Renderer);
        SDL_SetRenderTarget(Renderer, m_Canvas);
    }

    DrawCommon(
        t->button_left_rc, t->button_middle_rc, t->button_right_rc,
        t->button_left_disabled_rc, t->button_middle_disabled_rc, t->button_right_disabled_rc,
        t->button_left_focused_rc, t->button_middle_focused_rc, t->button_right_focused_rc,
        t->button_left_hover_focused_rc, t->button_middle_hover_focused_rc, t->button_right_hover_focused_rc,
        t->button_left_pushed_focused_rc, t->button_middle_pushed_focused_rc, t->button_right_pushed_focused_rc,
        t->button_left_hover_rc, t->button_middle_hover_rc, t->button_right_hover_rc,
        t->button_left_pushed_rc, t->button_middle_pushed_rc, t->button_right_pushed_rc
    );

    // ================================================================
    // Rysujemy tekst i ewentualną ikonkę
    // ================================================================
    TWindow* w = Tilc::GameObject->GetContext()->m_Window;
    SDL_Texture* TextureMap = t->GuiTextureMap1;
    Tilc::Gui::TFont* Font = t->DefaultFont;
    Font->SetColor({ 255, 255, 255, 255 });

    Tilc::TExtString caption = GetText();
    SDL_FRect rc = m_Position;
    if (Font)
    {
        //std::string s = caption + ": " + std::to_string(m_State);
        //DefaultFont->DrawString(GetRenderer(), s.c_str(), &rc, Align_CenterVertical | Align_CenterHorizontal);
        Font->DrawString(GetRenderer(), caption.c_str(), &rc, Align_CenterVertical | Align_CenterHorizontal);
    }

    // IKONKA
    /*
    int textDrawingAttribs = DT_SINGLELINE | DT_VCENTER | DT_CENTER;
    if (this->_imageList) {
        int image_index = this->_normalStateImageIndex;
        switch (this->_state) {
        case CONTROL_STATE_HOVER:
            image_index = this->_hoverStateImageIndex;
            break;
        case CONTROL_STATE_PUSHED:
            image_index = this->_pushedStateImageIndex;
            break;
        case CONTROL_STATE_FOCUSED:
            image_index = this->_focusedStateImageIndex;
            break;
        case CONTROL_STATE_DISABLED:
            image_index = this->_disabledStateImageIndex;
            break;
        }
        // TUTAJ KOD rysujący ikonkę
        if (image_index >= 0) {
            textDrawingAttribs = DT_SINGLELINE | DT_VCENTER | DT_LEFT;
            int yDelta = 0;
            if (this->_state & CONTROL_STATE_PUSHED) {
                yDelta = -1;
            }
            this->_imageList->transparentBitblt(this->canvas->getDC(), rc.left + 2, rc.top + yDelta + (this->height - this->_imageList->imHeight) / 2, image_index);
            rc.left += this->_imageList->imWidth + 4;
        }
    }
    */
    // ================================================================
    // Koniec rysowania tekstu
    // ================================================================

    if (m_Canvas)
    {
        SDL_SetRenderTarget(Renderer, OldRenderTarget);
    }
    m_NeedUpdate = ENeedUpdate::ENU_None;
}

bool Tilc::Gui::TButton::CommonKeyProcessing(const SDL_Event& Event)
{
    m_NeedUpdate = ENeedUpdate::ENU_None;
    return true;
}

void Tilc::Gui::TButton::SetImagesTextureMap(SDL_Texture* ImagesTextureMap, int ImagesW, int ImagesH,
    int NormalStateImageIndex, int HoverStateImageIndex, int PushedStateImageIndex, int FocusedStateImageIndex, int DisabledStateImageIndex)
{
    m_ImagesTextureMap = ImagesTextureMap;
    if (m_ImagesTextureMap)
    {
        int w = ImagesTextureMap->w;
        int h = ImagesTextureMap->h;
        int ImagesPerRow = w / ImagesW;
        int Col, Row;

        if (NormalStateImageIndex >= 0)
        {
            Col = NormalStateImageIndex % ImagesPerRow;
            Row = NormalStateImageIndex / ImagesPerRow;
            m_NormalStateImageRect = {Col * ImagesW, Row * ImagesH, ImagesW, ImagesH};
        }
        if (HoverStateImageIndex >= 0)
        {
            Col = HoverStateImageIndex % ImagesPerRow;
            Row = HoverStateImageIndex / ImagesPerRow;
            m_HoverStateImageRect = {Col * ImagesW, Row * ImagesH, ImagesW, ImagesH};
        }
        if (PushedStateImageIndex >= 0)
        {
            Col = PushedStateImageIndex % ImagesPerRow;
            Row = PushedStateImageIndex / ImagesPerRow;
            m_PushedStateImageRect = {Col * ImagesW, Row * ImagesH, ImagesW, ImagesH};
        }
        if (FocusedStateImageIndex >= 0)
        {
            Col = FocusedStateImageIndex % ImagesPerRow;
            Row = FocusedStateImageIndex / ImagesPerRow;
            m_FocusedStateImageRect = {Col * ImagesW, Row * ImagesH, ImagesW, ImagesH};
        }
        if (DisabledStateImageIndex >= 0)
        {
            Col = DisabledStateImageIndex % ImagesPerRow;
            Row = DisabledStateImageIndex / ImagesPerRow;
            m_DisabledStateImageRect = {Col * ImagesW, Row * ImagesH, ImagesW, ImagesH};
        }
    }
    else
    {
        RemoveImage();
    }
    Invalidate();
}

void Tilc::Gui::TButton::RemoveImage()
{
    m_NormalStateImageRect = {};
    m_HoverStateImageRect = {};
    m_PushedStateImageRect = {};
    m_FocusedStateImageRect = {};
    m_DisabledStateImageRect = {};
}
