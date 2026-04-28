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
            TClipboard() {};
            virtual ~TClipboard() {};

            virtual uint64_t GetClipboardSequenceNumber() { return 0; }
            virtual void GetClipboardFormatNameA(TExtString& name, unsigned int format) {};
            // Funcja zwraca uchwyt do okna, które ostatnio wywołało funkcję OpenClipboard i jeszcze nie
            // Zamknęło schowka przy pomocy funkcji CloseClipboard.
            virtual void GetOpenClipboardWindow() {}
            // Funkcja zwraca uchwyt do okna, które ostatnio umieściło w schowku dane.
            virtual void* GetClipboardOwner() { return nullptr; }
            virtual bool EmptyClipboard() { return false; }
            // Zwraca true, jeśli podany format jest w schowku dostępny
            virtual bool IsClipboardFormatAvailable(void* Window, unsigned int format) { return false; }
            // Zwraca łańcuch integerów reprezentujących aktualnie dostępne w schowku formaty
            // Inty rozdzielone są podanym separatorem i są zapisane szesnastkowo.
            virtual TExtString GetAllAvailableFormats(void* Window, TExtString separator = " ") { return ""; }
            // Zwraca łańcuch makrodefinicji preprocesora zdefiniowanych w pliku winuser.h
            // odpowiadających aktualnie dostępnym w schowku formatom. Jeśli parametr withNumbers jest
            // równy TRUE, to obok nazwy makrodefinicji w nawiasie umieszczany jest numer szesnastkowy
            // odpowiadający danemu formatowi.
            virtual TExtString GetAllAvailableFormatsDefines(void* Window, const TExtString& separator = " ", bool withNumbers = true) { return ""; }

            // umieszcza w schowku tekst
            virtual bool CopyTextToClipboard(void* Window, const TExtString& s) { return false; }
            // pobiera ze schowka tekst (o ile taki jest dostępny). Jeśli tekst nie jest dostępny, to
            // zwracany jest pusty łańcuch.
            virtual TExtString GetTextFromClipboard(void* Window) { return ""; }
        };
    }
}
