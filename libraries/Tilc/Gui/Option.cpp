#include "Tilc/Gui/Option.h"
#include "Tilc/Gui/OptionGroup.h"
#include "Tilc/Gui/GuiControl.h"
#include "Tilc/Gui/Theme.h"
#include "Tilc/Gui/Font.h"
#include "Tilc/Gui/StyledWindow.h"
#include "Tilc/Game.h"

Tilc::Gui::TOption::TOption(TGuiControl* parent, const Tilc::TExtString& name, const SDL_FRect& position, const Tilc::TExtString& text, bool checked)
    : Tilc::Gui::TCheckbox(parent, name, position, text, checked, Tilc::Gui::EControlType::ECT_Option)
{
}

Tilc::Gui::TOption::TOption(TGuiControl* parent, const Tilc::TExtString& name, const SDL_FRect& position, const Tilc::TExtString& checkedStateText, const Tilc::TExtString& uncheckedStateText, bool checked, int textPlacement, int textSpacing)
    : Tilc::Gui::TCheckbox(parent, name, position, checkedStateText, uncheckedStateText, checked, textPlacement, textSpacing, Tilc::Gui::EControlType::ECT_Option)
{
}

Tilc::Gui::TOption::~TOption() {
}

void Tilc::Gui::TOption::Draw()
{
    TTheme* t = Tilc::GameObject->GetContext()->m_Theme;
    SDL_Texture* OldRenderTarget{ nullptr };

    if (m_Canvas)
    {
        OldRenderTarget = SDL_GetRenderTarget(Renderer);
        SDL_SetRenderTarget(Renderer, m_Canvas);
    }

    // ================================================================
    // Draw option
    // ================================================================
    int option_width = t->option_checked_rc.w;
    int option_height = t->option_checked_rc.h;
    float x{}, y = (m_Position.h - option_height) / 2.0f;
    if (m_TextPlacement == CONTROL_TEXT_PLACEMENT_LEFT)
    {
        x = m_TextSize.w + m_TextSpacing;
    }

    SDL_FRect Position = GetRealPosition();
    Position.x = x;
    Position.y = y;
    DrawCommon(
        Position,
        m_IsChecked ? t->option_checked_rc : t->option_rc,
        m_IsChecked ? t->option_checked_disabled_rc : t->option_disabled_rc,
        m_IsChecked ? t->option_checked_focused_rc : t->option_focused_rc,
        m_IsChecked ? t->option_checked_focused_rc : t->option_focused_rc, // hover_focused, the same as above
        m_IsChecked ? t->option_checked_focused_rc : t->option_focused_rc, // pushed_focused, the same as above
        m_IsChecked ? t->option_checked_hover_rc : t->option_hover_rc,
        m_IsChecked ? t->option_checked_rc : t->option_rc // checked_pushed the same as in normal state
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
        if (option_width)
        {
            x += option_width + m_TextSpacing;
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

bool Tilc::Gui::TOption::OnMouseButtonUp(const SDL_Event& event)
{
    if (!m_Visible) return false;
    if (OtherControlCapturedMouse())
    {
        return false;
    }

    TGuiControl::OnMouseButtonUp(event);

    if (PointIn(event.button.x, event.button.y))
    {
        if (!m_IsChecked)
        {
            if (m_OptionGroup)
            {
                m_OptionGroup->CheckOption(this);
            }
            else
            {
                ToggleCheckedState();
            }
            Invalidate();
        }
        return true;
    }

    return false;
}

bool Tilc::Gui::TOption::OnKeyUp(const SDL_Event& event)
{
    if (event.key.key == SDLK_SPACE)
    {
        if (!m_IsChecked)
        {
            if (m_OptionGroup)
            {
                m_OptionGroup->CheckOption(this);
            }
            else
            {
                ToggleCheckedState();
            }
        }
        return true;
    }

    return false;
}
