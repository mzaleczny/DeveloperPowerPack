#pragma once

#include "Tilc/DllGlobals.h"
#include "SDL3/SDL.h"
#include "Tilc/Gui/TextField.h"
#include <vector>

namespace Tilc
{
    namespace Gui
    {
        class TStyledWindow;

        namespace Helpers {
            class TTextLayoutCache;
            class THbTextLayoutCache;
        }


        class DECLSPEC TMultilineTextField : public TTextField
        {
        public:
            TMultilineTextField(TGuiControl* parent, const Tilc::TExtString& name, const SDL_FRect& position, const TExtString& text = "", bool tabStop = true);
            virtual ~TMultilineTextField();

            virtual void Draw() override;

            int GetLastVisibleCharPosInLine(int StartChar);
            virtual void PositionCaretNearClickedPoint(float localX, float localY) override;
            // Updates Caret position
            void UpdateCaretPos() override;
            virtual SDL_FPoint CalculateCaretPos() override;
            virtual SDL_FPoint CalculateCharPos(int CurrentChar, int& Result) override;
            virtual void UpdateSelection(unsigned int vkKey, int lastCaretAtChar, int PrevLineNumber, int LineStartPos, int LineEndPos, bool& updateCaretPos, bool& redraw) override;
            // Updates position within text with use of m_CaretAtChar
            virtual void UpdateCursorPosition(unsigned int vkKey, bool& updateCaretPos, bool& redraw) override;

            // Usuwa zaznaczenie (bez usunięcia tekstu, który ono wskazywało) i ewentualnie odrysowuje
            // kontrolkę
            virtual void ClearSelection(bool redraw = true) override;
            // Zwraca TRUE, jeśli w kontrolce jest ustawion zaznaczenie.
            virtual bool IsSelection() override
            {
                return m_SelStart != m_SelEnd || m_SelectionLineStart != m_SelectionLineEnd;
            }
            // Wygaś poniższą funkcję ponieważ jest ona wywoływana w klasie macierzystej i dla tej kontrolki ta funkcja z klasy macierzystej spowodowałaby endless loop
            virtual SDL_FRect CalculateSelectionRect() override {
                return{};
            }

            virtual void MoveCaretOneCharLeft() override;
            virtual void MoveCaretOneCharRight() override;

            // Funkcje obsługi zdarzeń
            virtual bool OnMouseMove(const SDL_Event& event) override;
            virtual bool OnMouseButtonDown(const SDL_Event& event) override
            {
                TTextField::OnMouseButtonDown(event);
                m_SelectionLineStart = m_SelectionLineEnd = m_SelBeginLineNumber = m_CurrentLine;
                return true;
            };
            virtual bool OnMouseButtonUp(const SDL_Event& event) override
            {
                return TTextField::OnMouseButtonUp(event);
            };
            virtual bool OnKeyDown(const SDL_Event& event) override;
            virtual bool OnTextInput(const SDL_Event& event) override;
            // Odrysowuje linię o podanym indeksie w buforze przechowującym cały tekst widoczny w kontrolce. Przed wypisaniem tekstu tej linii, jest ona czyszczona kolorem przezrozystym.
            void RedrawLineInTextTextureBuffer(int LineNumber);
            // Odświeża bufor przechowujący cały tekst widoczny w kontrolce tak, że zawiera on wszystkie linie jedna pod drugą, ale bez linii o podanym numerze. Przed odpowiednim skopiowaniem
            // tekstu bufor jest czyszczony kolorem przezroczystym. W wyniku tej operacji na dole bufora zostaje więcwolne miejsce na kolejną linię. Któą należy wyrysować po wywołaniu tej
            // funkcji.
            void RedrawTextTextureBufferWithoutLine(int WithoutLineNumber);
            void RedrawTextTextureBufferStartingAtLine(int StartLineNumber);
            int GetNumberOfVisibleLines() const;
        protected:
            SDL_Texture* m_TextTexture{};
            Tilc::Gui::Helpers::THbTextLayoutCache* m_HbTextLayoutCache;
            // This controls uses its own texture for rendering where there is x = 0, y = 0 and not relative to parent
            SDL_FRect m_RealPosition;
            bool m_RenderedTextToUpdate{ true };
            // Indeksy linijek początku i końca zaznaczenia
            int m_SelectionLineStart{};
            int m_SelectionLineEnd{};
        };
    }
}
