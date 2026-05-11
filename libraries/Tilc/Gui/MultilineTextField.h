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

            virtual void Draw();
            void DrawTextAndCacheIt();

            int GetLastVisibleCharPosInLine(int StartChar);
            virtual void PositionCaretNearClickedPoint(float localX, float localY);
            virtual SDL_FPoint CalculateCaretPos() override;

        protected:
            std::vector<std::pair<int, Tilc::TExtString>> m_DisplayedLines;
            int m_CurrentLine = 0;
        };
    }
}
