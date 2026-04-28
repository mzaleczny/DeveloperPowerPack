#pragma once

#include "Tilc/DllGlobals.h"
#include "SDL3/SDL.h"
#include "Tilc/Utils/ExtString.h"
#include "Tilc/Gui/Clipboard.h"
#include <list>

namespace Tilc::OS::Gui
{
    class TStyledWindow;

    class DECLSPEC TWindowsClipboard : public Tilc::Gui::TClipboard
    {
    public:
        TWindowsClipboard();
        virtual ~TWindowsClipboard();

        virtual void GetClipboardFormatNameA(TExtString& name, unsigned int format) override;
        virtual bool EmptyClipboard() override;
        virtual bool IsClipboardFormatAvailable(void* Window, unsigned int format) override;
        virtual TExtString GetAllAvailableFormats(void* Window, TExtString separator = " ") override;
        virtual TExtString GetAllAvailableFormatsDefines(void* Window, const TExtString& separator = " ", bool withNumbers = true) override;
        virtual bool CopyTextToClipboard(void* Window, const TExtString& s) override;
        TExtString GetTextFromClipboard(void* Window) override;

    protected:
        // lista dostępnych w schowku formatów
        std::list<unsigned int> m_AvailableFormats;

        // pobiera wszystkie dostępne w schowku formaty
        bool GetAvailableFormats(void* Window);
    };
}
