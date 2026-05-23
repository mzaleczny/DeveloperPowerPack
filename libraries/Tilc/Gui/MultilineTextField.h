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
        }


        class DECLSPEC TMultilineTextField : public TTextField
        {
        public:
            TMultilineTextField(TGuiControl* parent, const Tilc::TExtString& name, const SDL_FRect& position, const TExtString& text = "", bool tabStop = true);
            virtual ~TMultilineTextField();

            virtual void Draw() override;
            void UpdateCache();

            int GetLastVisibleCharPosInLine(int StartChar);
            virtual void PositionCaretNearClickedPoint(float localX, float localY) override;
            // Updates Caret position
            void UpdateCaretPos() override;
            virtual SDL_FPoint CalculateCaretPos() override;
            virtual SDL_FPoint CalculateCharPos(int CurrentChar, int& Result) override;
            virtual void UpdateSelection(unsigned int vkKey, int lastCaretAtChar, int LineStartPos, int LineEndPos, bool& updateCaretPos, bool& redraw) override;
            // Updates position within text with use of m_CaretAtChar
            virtual void UpdateCursorPosition(unsigned int vkKey, bool& updateCaretPos, bool& redraw) override;

            virtual void MoveCaretOneCharLeft() override;
            virtual void MoveCaretOneCharRight() override;

            // Funkcje obsługi zdarzeń
            virtual bool OnMouseMove(const SDL_Event& event) override
            {
                return __super::OnMouseMove(event);
            };
            virtual bool OnMouseButtonDown(const SDL_Event& event) override
            {
                return __super::OnMouseButtonDown(event);
            };
            virtual bool OnMouseButtonUp(const SDL_Event& event) override
            {
                return __super::OnMouseButtonUp(event);
            };
            virtual bool OnKeyDown(const SDL_Event& event) override;
            virtual bool OnTextInput(const SDL_Event& event) override;

            std::vector<SDL_FRect> CalculateSelectionRects();

            bool IsCharPosWithinCurrentLine(int Pos)
            {
                return (Pos >= m_DisplayedLines[m_CurrentLine].first && Pos <= m_DisplayedLines[m_CurrentLine].first + m_DisplayedLines[m_CurrentLine].second.length());
            }

        protected:
            Tilc::Gui::Helpers::TTextLayoutCache* m_TextLayoutCache;
            std::vector<std::pair<int, Tilc::TExtString>> m_DisplayedLines;
            // This controls uses its own texture for rendering where there is x = 0, y = 0 and not relative to parent
            SDL_FRect m_RealPosition;
            int m_CurrentLine = 0;
            bool m_DoUpdateCache{true};
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
