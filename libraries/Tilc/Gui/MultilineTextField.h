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

        class DECLSPEC TMultilineTextField : public TTextField
        {
        public:
            TMultilineTextField(TGuiControl* parent, const Tilc::TExtString& name, const SDL_FRect& position, const TExtString& text = "", bool tabStop = true);
            virtual ~TMultilineTextField();

            virtual void Draw() override;
            void UpdateDisplayLinesCache();

            int GetLastVisibleCharPosInLine(int StartChar);
            virtual void PositionCaretNearClickedPoint(float localX, float localY) override;
            virtual SDL_FPoint CalculateCaretPos() override;
            virtual SDL_FPoint CalculateCharPos(int CurrentChar, int& Result) override;
            virtual void UpdateSelection(unsigned int vkKey, int lastCaretAtChar, bool& updateCaretPos, bool& redraw) override;
            virtual void UpdateCursorPosition(unsigned int vkKey, bool& updateCaretPos, bool& redraw) override;

            virtual void MoveCaretOneCharLeft() override;
            virtual void MoveCaretOneCharRight() override;

            // Funkcje obsługi zdarzeń
            virtual bool OnKeyDown(const SDL_Event& event) override;
            virtual bool OnTextInput(const SDL_Event& event) override;

            std::vector<SDL_FRect> CalculateSelectionRects();
        protected:
            std::vector<std::pair<int, Tilc::TExtString>> m_DisplayedLines;
            int m_CurrentLine = 0;
            bool m_RefreshDisplayLinesCache{true};
            // Przechowuj true, jeśli karetka jest na końcu linii. Jest po to, by rozróżnić to samo połozenie karetki wewnątrz bufora dla pozycji na końcu jednej linii i na początku następnej.
            // W obu tych przypadkach zmienna m_CaretAtPos ma tę samą wartość, ale położeni karetki może być w dwóch różnych punkatch.
            bool m_CaretAtEndOfLine{};
            void DeleteCacheFromCurrentLine();
            void DeleteAndRefreshAllCache();
            int GetLineForCurrentCaretPos();
            bool IsAtEndOfLine();
            bool IsAtLineBreak();
        };
    }
}
