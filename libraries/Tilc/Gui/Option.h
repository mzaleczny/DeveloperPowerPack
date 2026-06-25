#pragma once

#include "Tilc/Gui/Checkbox.h"
#include "Tilc/Utils/ExtString.h"
#include <list>
#include <functional>


namespace Tilc::Gui
{
    class TOptionGroup;

    class DECLSPEC TOption : public TCheckbox
    {
    public:
        TOption(TGuiControl* parent, const Tilc::TExtString& name, const SDL_FRect& position, const Tilc::TExtString& text, bool checked);
        TOption(TGuiControl* parent, const Tilc::TExtString& name, const SDL_FRect& position, const Tilc::TExtString& checkedStateText, const Tilc::TExtString& uncheckedStateText, bool checked, int textPlacement = CONTROL_TEXT_PLACEMENT_RIGHT, int textSpacing = CONTROL_DEFAULT_SPACING);
        virtual ~TOption();

        // Funkcje obsługi zdarzeń
        virtual void Draw() override;
        virtual bool OnMouseButtonUp(const SDL_Event& event) override;
        virtual bool OnKeyUp(const SDL_Event& event) override;

        void AddToGroup(TOptionGroup* Group)
        {
            m_OptionGroup = Group;
        }
    protected:
        TOptionGroup* m_OptionGroup{};
    };
}
