#pragma once

#include "Tilc/Gui/GuiControl.h"
#include "Tilc/Utils/ExtString.h"
#include <list>
#include <functional>


namespace Tilc::Gui
{

    constexpr const int CHECKBOX_DEFAULT_SPACING = 4;
    constexpr const int CHECKBOX_TEXT_PLACEMENT_LEFT = 1;
    constexpr const int CHECKBOX_TEXT_PLACEMENT_RIGHT = 2;

    /*
    class DECLSPEC TCheckbox : public TGuiControl {
    public:
        TCheckbox(TGuiControl* parent, const Tilc::TExtString& name, const SDL_FRect& position, const Tilc::TExtString& text, bool checked);
        TCheckbox(TGuiControl* parent, const Tilc::TExtString& name, const SDL_FRect& position, const Tilc::TExtString& checkedStateText, const Tilc::TExtString& uncheckedStateText, bool checked, int textPlacement = CHECKBOX_TEXT_PLACEMENT_RIGHT, int textSpacing = CHECKBOX_DEFAULT_SPACING);
        virtual ~TCheckbox();

        virtual void SetText(const Tilc::TExtString& checkedStateText, const Tilc::TExtString& uncheckedStateText, bool redraw = true);
        virtual void SetText(const Tilc::TExtString& text) override;

        // zwraca przechowywany w kontrolce tekst
        virtual Tilc::TExtString GetText() override;
        inline bool IsChecked() { return this->m_IsChecked; }
        void SetChecked(bool checked);
        void ToggleCheckedState();

        // Umieszcza focus na tej kontrolce a dokładnie:
        //      Jeśli aktywną kontrolką okna Parent nie jest ta kontrolka, to jest wywoływane metoda
        //      ParentWnd->setActiveControl(this) tego okna.
        //      Przełącza stan kontrolki w tryb CmzButton_STATE_FOCUSED.
        virtual void Focus() override;
        // Usuwa focus z tej kontrolki a dokładnie:
        //      NULL-uje wskaźnik aktywnej kontrolki w ParentWindow
        //      Przełącza kontrolkę w stan CmzButton_STATE_HOVER lub CmzButton_STATE_NORMAL
        //      w zależności od aktualnego położenia myszy.
        virtual void LooseFocus();

        // Funkcje obsługi zdarzeń
        virtual void Draw() override;
        virtual bool OnMouseMove(const SDL_Event& event) override;
        virtual bool OnMouseButtonDown(const SDL_Event& event) override;
        virtual bool OnMouseButtonUp(const SDL_Event& event) override;

        virtual bool OnKeyDown(const SDL_Event& event) override;
        virtual bool OnKeyUp(const SDL_Event& event) override;
    protected:
        Tilc::TExtString m_CheckedStateText;
        Tilc::TExtString m_UncheckedStateText;
        int m_TextSpacing;
        int m_TextPlacement;
        SDL_FRect m_TextSize;
        bool m_IsChecked;

        virtual void CommonInit(const Tilc::TExtString& checkedStateText, const Tilc::TExtString& uncheckedStateText, bool checked, int width, int textPlacement, int textSpacing);
    };
    */
}
