#include "Tilc/Gui/Caret.h"
#include "Tilc/Globals.h"
#include "Tilc/Gui/StyledWindow.h"

Tilc::Gui::TCaret::TCaret(Tilc::Gui::TStyledWindow* wnd, int x, int y, int width, int height)
    : m_ParentWindow(wnd), m_BlinkingDelay(500.0f), m_BlinkingElapsedTime(0.0f), m_Visible(false), m_Active(false),
      m_Width(width), m_Height(height), m_Color({0x00, 0x00, 0x00, 0xff})
{
}

void Tilc::Gui::TCaret::Draw(SDL_Texture* Dest)
{
    if (!m_Active || !m_ParentWindow)
    {
        return;
    }

    if (m_Visible)
    {
        SDL_FRect rc = GetRect();
        SDL_Texture* OldRenderTarget = SDL_GetRenderTarget(m_ParentWindow->Renderer);
        SDL_SetRenderTarget(m_ParentWindow->Renderer, Dest);
        SDL_RenderFillRect(m_ParentWindow->Renderer, &rc);
        SDL_SetRenderTarget(m_ParentWindow->Renderer, OldRenderTarget);
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
