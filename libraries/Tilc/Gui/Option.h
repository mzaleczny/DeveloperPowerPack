#pragma once

#include "Tilc/Gui/Checkbox.h"
#include "Tilc/Utils/ExtString.h"
#include <list>
#include <functional>


namespace Tilc::Gui
{

    class DECLSPEC TOption : public TCheckbox
    {
    public:
        TOption(TGuiControl* parent, const Tilc::TExtString& name, const SDL_FRect& position, const Tilc::TExtString& text, bool checked);
        TOption(TGuiControl* parent, const Tilc::TExtString& name, const SDL_FRect& position, const Tilc::TExtString& checkedStateText, const Tilc::TExtString& uncheckedStateText, bool checked, int textPlacement = CONTROL_TEXT_PLACEMENT_RIGHT, int textSpacing = CONTROL_DEFAULT_SPACING);
        virtual ~TOption();

        // Funkcje obsługi zdarzeń
        virtual void Draw() override;
    };
}
