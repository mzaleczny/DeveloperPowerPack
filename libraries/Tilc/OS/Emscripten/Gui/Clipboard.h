#pragma once

#include "Tilc/DllGlobals.h"
#include "Tilc/Utils/ExtString.h"
#include "Tilc/Gui/Clipboard.h"
#include <list>
#include <SDL3/SDL.h>
#include "SDL3/SDL_clipboard.h"
#include <emscripten/emscripten.h>

extern "C" {
    EMSCRIPTEN_KEEPALIVE
    void HandlePasteEvent(const char* text);
}

namespace Tilc::OS::Gui
{
    class TStyledWindow;

    class DECLSPEC TEmscriptenClipboard : public Tilc::Gui::TClipboard
    {
    public:
        TEmscriptenClipboard();
        virtual ~TEmscriptenClipboard();

        virtual bool CopyTextToClipboard(void* Window, const Tilc::TExtString& s) override;
    };
}
