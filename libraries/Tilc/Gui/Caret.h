#pragma once

#include "Tilc/DllGlobals.h"
#include "SDL3/SDL.h"

namespace Tilc
{
    namespace Gui
    {
        class TStyledWindow;

        class DECLSPEC TCaret
        {
        public:
            // współrzędne karetki w oknie
            int m_X, m_Y;
            // współrzędne karetki w aktywnej kontrolce
            int m_ControlX, m_ControlY;
            int m_Width, m_Height;
            // czy karetka jest aktywna, tzn. czy migotanie ma miejsce
            bool m_Active;
            TStyledWindow* m_ParentWindow;

            TCaret(TStyledWindow* wnd, int x, int y, int width = 2, int height = 20);

            virtual void Draw();
            void Update(float DeltaTime);
            inline void setColor(SDL_Color color)
            {
                m_Color = color;
            }
            virtual inline SDL_FRect GetRect()
            {
                return { static_cast<float>(m_X), static_cast<float>(m_Y), static_cast<float>(m_Width), static_cast<float>(m_Height) };
            }
            inline void Show() { m_Visible = true; }
            inline void Hide() { m_Visible = false; }
            inline bool IsVisible() const { return m_Visible; }

        protected:
            SDL_Renderer* m_Renderer{};
            // szybkość migotania w milisekundach
            float m_BlinkingDelay;
            // czas ostatniej zmiany widoczności karetki
            float  m_BlinkingElapsedTime;
            // czy karetka jest aktualnie widoczna
            bool m_Visible;
            // kolor karetki
            SDL_Color m_Color;
        };
    }
}
