#include "Tilc/Gui/Checkbox.h"
#include "Tilc/Gui/Button.h"
#include "Tilc/Gui/Theme.h"
#include "Tilc/Gui/Font.h"
#include "Tilc/Gui/StyledWindow.h"
#include "Tilc/Game.h"

void Tilc::Gui::TCheckbox::CommonInit(const Tilc::TExtString& checkedStateText, const Tilc::TExtString& uncheckedStateText, bool checked, int textPlacement, int textSpacing)
{
    m_TextPlacement = textPlacement;
    m_TextSpacing = textSpacing;
    m_IsChecked = checked;

    Tilc::Gui::TTheme* Theme = GetTheme();
    SetText(checkedStateText, uncheckedStateText, false);
    int min_height = Theme->checkbox_rc.h + 2 + 2;
    if (min_height < m_TextSize.h + 2 + 2)
    {
        min_height = m_TextSize.h + 2 + 2;
    }
    int min_width = Theme->checkbox_rc.w + m_TextSpacing + m_TextSize.w + 2 + 2;
    if (m_Position.w < min_width || m_Position.h < min_height)
    {
        SetSize(min_width, min_height);
    }
    m_TabStop = true;
}

Tilc::Gui::TCheckbox::TCheckbox(TGuiControl* parent, const Tilc::TExtString& name, const SDL_FRect& position, const Tilc::TExtString& text, bool checked, Tilc::Gui::EControlType Type)
    : Tilc::Gui::TGuiControl(parent, name, position, Type)
{
    CommonInit(text, text, checked, CONTROL_TEXT_PLACEMENT_RIGHT, CONTROL_DEFAULT_SPACING);
}

Tilc::Gui::TCheckbox::TCheckbox(TGuiControl* parent, const Tilc::TExtString& name, const SDL_FRect& position, const Tilc::TExtString& checkedStateText, const Tilc::TExtString& uncheckedStateText, bool checked, int textPlacement, int textSpacing, Tilc::Gui::EControlType Type)
    : Tilc::Gui::TGuiControl(parent, name, position, Type)
{
    CommonInit(checkedStateText, uncheckedStateText, checked, textPlacement, textSpacing);
}

Tilc::Gui::TCheckbox::~TCheckbox() {
}

void Tilc::Gui::TCheckbox::SetText(const Tilc::TExtString& checkedStateText, const Tilc::TExtString& uncheckedStateText, bool redraw)
{
    m_CheckedStateText = checkedStateText;
    m_UncheckedStateText = uncheckedStateText;
    
    Tilc::TExtString text = m_CheckedStateText;
    Tilc::Gui::TTheme* Theme = GetTheme();
    int Width, Height;
    m_TextSize = {};
    if (Theme->DefaultFont)
    {
        Theme->DefaultFont->GetTextSize(text.c_str(), Width, Height);
        m_TextSize.w = static_cast<float>(Width);
        m_TextSize.h = static_cast<float>(Height);

        text = m_UncheckedStateText;
        Theme->DefaultFont->GetTextSize(text.c_str(), Width, Height);
        if (Width > m_TextSize.w)
        {
            m_TextSize.w = static_cast<float>(Width);
        }
        if (Height > m_TextSize.h)
        {
            m_TextSize.h = static_cast<float>(Height);
        }
    }
    
    if (IsChecked())
    {
        m_Text = m_CheckedStateText;
    }
    else
    {
        m_Text = m_UncheckedStateText;
    }
    
    m_Text.RemoveCharsWithCodeLessThan(32);
    if (redraw)
    {
        Invalidate();
    }
}

void Tilc::Gui::TCheckbox::SetText(const Tilc::TExtString& text)
{
    SetText(text, text, true);
}

void Tilc::Gui::TCheckbox::Draw()
{
    TTheme* t = Tilc::GameObject->GetContext()->m_Theme;
    SDL_Texture* OldRenderTarget{ nullptr };

    if (m_Canvas)
    {
        OldRenderTarget = SDL_GetRenderTarget(Renderer);
        SDL_SetRenderTarget(Renderer, m_Canvas);
    }

    // ================================================================
    // Draw checkbox
    // ================================================================
    int checkbox_width = t->checkbox_checked_rc.w;
    int checkbox_height = t->checkbox_checked_rc.h;
    float x{}, y = (m_Position.h - checkbox_height) / 2.0f;
    if (m_TextPlacement == CONTROL_TEXT_PLACEMENT_LEFT)
    {
        x = m_TextSize.w + m_TextSpacing;
    }

    SDL_FRect Position = GetRealPosition();
    Position.x = x;
    Position.y = y;
    DrawCommon(
        Position,
        m_IsChecked ? t->checkbox_checked_rc : t->checkbox_rc,
        m_IsChecked ? t->checkbox_checked_disabled_rc : t->checkbox_disabled_rc,
        m_IsChecked ? t->checkbox_checked_focused_rc : t->checkbox_focused_rc,
        m_IsChecked ? t->checkbox_checked_focused_rc : t->checkbox_focused_rc, // hover_focused, the same as above
        m_IsChecked ? t->checkbox_checked_focused_rc : t->checkbox_focused_rc, // pushed_focused, the same as above
        m_IsChecked ? t->checkbox_checked_hover_rc : t->checkbox_hover_rc,
        m_IsChecked ? t->checkbox_checked_rc : t->checkbox_rc // checked_pushed the same as in normal state
    );
    // ================================================================
    // ================================================================

    // ================================================================
    // Draw label
    // ================================================================
    Position = GetRealPosition();
    x = 0;
    if (m_TextPlacement == CONTROL_TEXT_PLACEMENT_RIGHT)
    {
        x = 0;
        if (checkbox_width)
        {
            x += checkbox_width + m_TextSpacing;
        }
    }
    Position.x += x;
    t->DefaultFont->DrawString(GetRenderer(), m_Text.c_str(), &Position, Align_CenterVertical | Align_Left);
    // ================================================================
    // ================================================================

    if (m_Canvas)
    {
        SDL_SetRenderTarget(Renderer, OldRenderTarget);
    }
    m_NeedUpdate = ENeedUpdate::ENU_None;
}

bool Tilc::Gui::TCheckbox::OnMouseButtonUp(const SDL_Event& event)
{
    if (!m_Visible) return false;
    if (OtherControlCapturedMouse())
    {
        return false;
    }

    TGuiControl::OnMouseButtonUp(event);

    if (PointIn(event.button.x, event.button.y))
    {
        ToggleCheckedState();
        Invalidate();
        return true;
    }

    return false;
}

void Tilc::Gui::TCheckbox::SetChecked(bool checked)
{
    m_IsChecked = checked;
    if (m_IsChecked)
    {
        m_Text = m_CheckedStateText;
    }
    else
    {
        m_Text = m_UncheckedStateText;
    }
    Invalidate();
}

bool Tilc::Gui::TCheckbox::OnKeyUp(const SDL_Event& event)
{
    if (event.key.key == SDLK_SPACE)
    {
        ToggleCheckedState();
        return true;
    }

    return false;
}
