#include "Tilc/OS/Emscripten/Gui/Clipboard.h"

#include <iostream>
#include <ostream>
#include <emscripten/emscripten.h>

extern "C" {
    EM_JS(void, JsCopyToClipboard, (const char* str), {
        const text = UTF8ToString(str);
        navigator.clipboard.writeText(text).catch(err => {
            console.error('Nie udało się skopiować do schowka: ', err);
        });
    });

    EM_JS(void, InitAppHandlers, (), {
        // 1. Blokujemy domyślne przewijanie strony dla klawiszy End, Home, PageUp, PageDown, Spacja
        window.addEventListener('keydown', async (e) => {
            const navigationKeys = ['End', 'Home', 'PageUp', 'PageDown', ' '];
            if (navigationKeys.includes(e.key)) {
                // Sprawdzamy czy użytkownik nie pisze w jakimś natywnym inputcie HTML (jeśli taki masz)
                if (document.activeElement.tagName !== 'INPUT' && document.activeElement.tagName !== 'TEXTAREA') {
                    e.preventDefault(); // Zapobiega przewijaniu strony przez przeglądarkę
                }
            }

            // 2. Obsługa wklejania (Ctrl+V / Cmd+V)
            if ((e.ctrlKey || e.metaKey) && e.key.toLowerCase() === 'v') {
                try {
                    const text = await navigator.clipboard.readText();
                    if (text) {
                        const length = lengthBytesUTF8(text) + 1;
                        const ptr = _malloc(length);
                        stringToUTF8(text, ptr, length);

                        _HandlePasteEvent(ptr);

                        _free(ptr);
                    }
                } catch (err) {
                    console.error('Błąd odczytu schowka:', err);
                }
            }
        });
    });

    EMSCRIPTEN_KEEPALIVE
    void HandlePasteEvent(const char* text) {
        SDL_SetClipboardText(text);
    }
}

Tilc::OS::Gui::TEmscriptenClipboard::TEmscriptenClipboard()
{
    InitAppHandlers();
}

Tilc::OS::Gui::TEmscriptenClipboard::~TEmscriptenClipboard()
{
}

bool Tilc::OS::Gui::TEmscriptenClipboard::CopyTextToClipboard(void* Window, const TExtString& s)
{
    JsCopyToClipboard(s.c_str());
    return true;
}
