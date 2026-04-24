#include "Tilc/Gui/Caret.h"
#include "Tilc/Globals.h"
#include "Tilc/Game.h"
#include "Tilc/Gui/StyledWindow.h"

Tilc::Gui::TCaret::TCaret(float x, float y, float width, float height)
    : m_BlinkingDelay(500.0f), m_BlinkingElapsedTime(0.0f), m_Visible(false), m_Active(false),
      m_Position({x, y, width, height}), m_Color({0x00, 0x00, 0x00, 0x00})
{
    m_Renderer = Tilc::GameObject->GetContext()->m_Window->GetRenderer();
}

void Tilc::Gui::TCaret::Draw()
{
    if (!m_Active)
    {
        return;
    }

    if (m_Visible)
    {
        SDL_SetRenderDrawColor(m_Renderer, m_Color.r, m_Color.g, m_Color.b, m_Color.a);
        SDL_RenderFillRect(m_Renderer, &m_Position);
    }
}

void Tilc::Gui::TCaret::Update(float DeltaTime)
{
    m_BlinkingElapsedTime += DeltaTime;
    if (m_BlinkingElapsedTime > m_BlinkingDelay)
    {
        m_BlinkingElapsedTime = 0.0f;
        m_Visible = !m_Visible;
    }
}
