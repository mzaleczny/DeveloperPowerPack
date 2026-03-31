#include "TStatePaused.h"
#include "Tilc/EventManager.h"
#include "Tilc/StateManager.h"
#include "Tilc/Game.h"
#include "Tilc/Window.h"
#include "SDL3_ttf/SDL_ttf.h"
#include "Tilc/Gui/TextBox.h"
#include "Tilc/Gui/Font.h"

TStatePaused::TStatePaused(Tilc::TStateManager* StateManager) : TBaseState(StateManager)
{
}

void TStatePaused::OnCreate()
{
	if (Tilc::GameObject)
	{
		TextBox = new Tilc::Gui::TTextBox(0.0f, 0.0f, "PAUSED", 0, Tilc::GameObject->GetFont("Tuffy"), 16);
	}
	if (TextBox)
	{
		TTF_Font* Font = TextBox->GetFont()->GetTTFFont();
		TTF_SetFontStyle(Font, TTF_STYLE_BOLD);
		TTF_SetFontSize(Font, 14.0f);
		SDL_Surface* TextSurface = TTF_RenderText_Solid(Font, "PAUSED", 6, { 0xff, 0xff, 0xff, SDL_ALPHA_OPAQUE });
		if (TextSurface)
		{
			SDL_Renderer* Renderer = Tilc::GameObject->GetContext()->m_Window->GetRenderer();
			if (Renderer)
			{
				TextTexture = SDL_CreateTextureFromSurface(Renderer, TextSurface);
				unsigned int WindowWidth = Tilc::GameObject->GetContext()->m_Window->GetWindowWidth();
				unsigned int WindowHeight = Tilc::GameObject->GetContext()->m_Window->GetWindowHeight();
				TextRect.w = TextSurface->w;
				TextRect.h = TextSurface->h;
				TextRect.x = (WindowWidth - TextRect.w) / 2.0f;
				TextRect.h = (WindowHeight - TextRect.h) / 2.0f;
			}
			SDL_DestroySurface(TextSurface);
			TextSurface = nullptr;
		}
	}

	Tilc::TEventManager* evMgr = Tilc::GameObject->GetContext()->m_EventManager;
	if (evMgr)
	{
		evMgr->AddCallback(Tilc::EStateType::Paused, "Key_P", &TStatePaused::Unpause, this);
	}
}

void TStatePaused::OnDestroy()
{
	if (TextBox)
	{
		delete TextBox;
		TextBox = nullptr;
	}
	Tilc::TEventManager* evMgr = Tilc::GameObject->GetContext()->m_EventManager;
	if (evMgr)
	{
		evMgr->RemoveCallback(Tilc::EStateType::Paused, "Key_P");
	}
}

void TStatePaused::Draw()
{
	if (TextTexture && TextRect.w > 0.0f)
	{

		static SDL_Renderer* Renderer = Tilc::GameObject->GetContext()->m_Window->GetRenderer();
		SDL_RenderTexture(Renderer, TextTexture, nullptr, &TextRect);
	}
}

void TStatePaused::Unpause(Tilc::TEventDetails* Details)
{
	m_StateManager->SwitchTo(Tilc::EStateType::Game);
}
