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
            // rectangle karetki w oknie
            SDL_FRect m_Position;
            // współrzędne karetki w aktywnej kontrolce
            int m_ControlX, m_ControlY;
            // czy karetka jest aktywna, tzn. czy migotanie ma miejsce
            bool m_Active;

            TCaret(float x = 0, float y = 0, float width = 2, float height = 20);

            virtual void Draw();
            void Update(float DeltaTime);
            inline void SetColor(SDL_Color color)
            {
                m_Color = color;
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
