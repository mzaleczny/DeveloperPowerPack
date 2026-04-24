#pragma once

#include "Tilc/DllGlobals.h"
#include "SDL3/SDL.h"
#include "Tilc/Utils/ExtString.h"

namespace Tilc
{
    namespace Gui
    {
        class TStyledWindow;

        class DECLSPEC TClipboard
        {
        public:
            bool CopyTextToClipboard(const TExtString& Text);
            TExtString GetTextFromClipboard();
        protected:
        };
    }
}
