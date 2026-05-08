#pragma once

#include "Tilc/DllGlobals.h"
#include "SDL3/SDL.h"
#include "Tilc/Gui/TextField.h"

namespace Tilc
{
    namespace Gui
    {
        class TStyledWindow;

        class DECLSPEC TMultilineTextField : public TTextField
        {
        public:
            TMultilineTextField(TGuiControl* parent, const Tilc::TExtString& name, const SDL_FRect& position, const TExtString& text = "", bool tabStop = true);
            virtual ~TMultilineTextField();

            virtual void Draw();

        protected:
        };
    }
}
