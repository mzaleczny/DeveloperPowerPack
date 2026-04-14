#include "Tilc/Gui/Button.h"
#include "Tilc/Gui/Theme.h"
#include "Tilc/Gui/Font.h"
#include "Tilc/Gui/StyledWindow.h"
#include "Tilc/Game.h"

Tilc::Gui::TButton::TButton(TGuiControl* parent, const Tilc::TExtString& name, const SDL_FRect& position, const Tilc::TExtString& text)
    : Tilc::Gui::TGuiControl(parent, name, position, Tilc::Gui::EControlType::ECT_RegularControl)
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
    TWindow* w = Tilc::GameObject->GetContext()->m_Window;
    SDL_Texture* TextureMap = t->GuiTextureMap1;
    Tilc::Gui::TFont* DefaultFont = t->DefaultFont;
    SDL_FRect rc, DestRect;
    SDL_FRect Position{ GetRealPosition() };
    float x{}, y{};
    SDL_Texture* OldRenderTarget{ nullptr };

    if (m_Canvas)
    {
        OldRenderTarget = SDL_GetRenderTarget(Renderer);
        SDL_SetRenderTarget(Renderer, m_Canvas);
    }

    SDL_FRect btn_left = t->button_left_rc;
    SDL_FRect btn_middle = t->button_middle_rc;
    SDL_FRect btn_right = t->button_right_rc;

    if (m_State & CONTROL_STATE_DISABLED)
    {
        btn_left = t->button_left_disabled_rc;
        btn_middle = t->button_middle_disabled_rc;
        btn_right = t->button_right_disabled_rc;
    }
    else if (m_State & CONTROL_STATE_FOCUSED)
    {
        btn_left = t->button_left_focused_rc;
        btn_middle = t->button_middle_focused_rc;
        btn_right = t->button_right_focused_rc;
        if (m_State & CONTROL_STATE_HOVER)
        {
            //std::cout << "HOVER: " << m_Name << std::endl;
            btn_left = t->button_left_hover_focused_rc;
            btn_middle = t->button_middle_hover_focused_rc;
            btn_right = t->button_right_hover_focused_rc;
        }
        else if (m_State & CONTROL_STATE_PUSHED)
        {
            btn_left = t->button_left_pushed_focused_rc;
            btn_middle = t->button_middle_pushed_focused_rc;
            btn_right = t->button_right_pushed_focused_rc;
        }
    }
    else if (m_State & CONTROL_STATE_HOVER)
    {
        btn_left = t->button_left_hover_rc;
        btn_middle = t->button_middle_hover_rc;
        btn_right = t->button_right_hover_rc;
    }
    else if (m_State & CONTROL_STATE_PUSHED)
    {
        btn_left = t->button_left_pushed_rc;
        btn_middle = t->button_middle_pushed_rc;
        btn_right = t->button_right_pushed_rc;
    }
    float frame_left_width = btn_left.w;
    float frame_right_width = btn_right.w;

    // ================================================================
    // Rysujemy tło button-a
    // ================================================================
    RenderTexture(TextureMap, &btn_left, x, y);
    x += btn_left.w;

    float middle_width = Position.w - frame_left_width - frame_right_width;
    RenderTexture(TextureMap, &btn_middle, x, y, middle_width, btn_middle.h);
    x += middle_width;

    RenderTexture(TextureMap, &btn_right, x, y);
    // ================================================================
    // Koniec rysowania tła button-a
    // ================================================================

    // ================================================================
    // Rysujemy tekst i ewentualną ikonkę
    // ================================================================
    Tilc::TExtString caption = GetText();
    rc = m_Position;
    if (DefaultFont)
    {
        //std::string s = caption + ": " + std::to_string(m_State);
        //DefaultFont->DrawString(GetRenderer(), s.c_str(), &rc, Align_CenterVertical | Align_CenterHorizontal);
        DefaultFont->DrawString(GetRenderer(), caption.c_str(), &rc, Align_CenterVertical | Align_CenterHorizontal);
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

void Tilc::Gui::TButton::Focus()
{
    Tilc::Gui::TStyledWindow* wnd = GetParentWindow();
    if (wnd && wnd->GetActiveControl() != this)
    {
        wnd->SetActiveControl(this);
        return;
    }
    AddState(CONTROL_STATE_FOCUSED);
}

void Tilc::Gui::TButton::LooseFocus()
{
    Tilc::Gui::TStyledWindow* wnd = GetParentWindow();
    float x = -1000.0;
    float y = -1000.0f;

    wnd->SetOnlyActiveControlPointer(nullptr);
    GetCurrentMousePosition(&x, &y);

    if (PointIn(x, y))
    {
        SetState(CONTROL_STATE_HOVER);
    }
    else
    {
        SetState(CONTROL_STATE_NORMAL);
    }
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
