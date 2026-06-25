#pragma once

#include "Tilc/Gui/GuiControl.h"
#include "Tilc/Utils/ExtString.h"
#include <list>
#include <functional>


namespace Tilc::Gui
{
    class DECLSPEC TCheckbox : public TGuiControl {
    public:
        TCheckbox(TGuiControl* parent, const Tilc::TExtString& name, const SDL_FRect& position, const Tilc::TExtString& text, bool checked, Tilc::Gui::EControlType Type = Tilc::Gui::EControlType::ECT_Checkbox);
        TCheckbox(TGuiControl* parent, const Tilc::TExtString& name, const SDL_FRect& position, const Tilc::TExtString& checkedStateText, const Tilc::TExtString& uncheckedStateText, bool checked, int textPlacement = CONTROL_TEXT_PLACEMENT_RIGHT, int textSpacing = CONTROL_DEFAULT_SPACING, Tilc::Gui::EControlType Type = Tilc::Gui::EControlType::ECT_Checkbox);
        virtual ~TCheckbox();

        virtual void SetText(const Tilc::TExtString& checkedStateText, const Tilc::TExtString& uncheckedStateText, bool redraw = true);
        virtual void SetText(const Tilc::TExtString& text) override;

        // zwraca przechowywany w kontrolce tekst
        inline bool IsChecked() { return this->m_IsChecked; }
        void SetChecked(bool checked);
        inline void ToggleCheckedState()
        {
            SetChecked(!m_IsChecked);
        }

        // Funkcje obsługi zdarzeń
        virtual void Draw() override;
        virtual bool OnMouseButtonUp(const SDL_Event& event) override;
        virtual bool OnKeyUp(const SDL_Event& event) override;
    protected:
        Tilc::TExtString m_CheckedStateText;
        Tilc::TExtString m_UncheckedStateText;
        int m_TextSpacing;
        int m_TextPlacement;
        SDL_FRect m_TextSize;
        bool m_IsChecked;

        virtual void CommonInit(const Tilc::TExtString& checkedStateText, const Tilc::TExtString& uncheckedStateText, bool checked, int textPlacement, int textSpacing);
    };
}
