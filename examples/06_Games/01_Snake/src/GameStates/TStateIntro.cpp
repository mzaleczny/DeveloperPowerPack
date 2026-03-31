#include "TStateIntro.h"
#include <SDL3_image/SDL_image.h>
#include "Tilc/Window.h"
#include "Tilc/Game.h"
#include "Tilc/Gui/Font.h"

TStateIntro::TStateIntro(Tilc::TStateManager* StateManager) : Tilc::TBaseState(StateManager)
{
	if (Tilc::GameObject)
	{
		Font = new Tilc::Gui::TFont(Tilc::GameObject->GetFont("Tuffy"));
	}
}

void TStateIntro::OnCreate()
{
	Tilc::TWindow* Window = Tilc::GameObject->GetContext()->m_Window;
	SDL_Renderer* Renderer = Window->GetRenderer();
	m_TimePassed = 0.0f;
	
	m_IntroTexture = IMG_LoadTexture(Renderer, "assets/background.jpg");
	SDL_GetTextureSize(m_IntroTexture, &m_IntroTextureRect.w, &m_IntroTextureRect.h);
	m_IntroTextureRect.x = (Window->GetWindowWidth() - m_IntroTextureRect.w) / 2.0f;
	m_IntroTextureRect.y = -m_IntroTextureRect.h - 50.0f;

	evMgr = Tilc::GameObject->GetContext()->m_EventManager;
	if (evMgr)
	{
		evMgr->AddCallback(Tilc::EStateType::Intro, "Intro_Continue", &TStateIntro::Continue, this);
	}
}

void TStateIntro::OnDestroy()
{
	if (Font)
	{
		delete Font;
		Font = nullptr;
	}
	if (m_IntroTexture)
	{
		SDL_DestroyTexture(m_IntroTexture);
		m_IntroTexture = nullptr;
	}
	if (evMgr)
	{
		evMgr->RemoveCallback(Tilc::EStateType::Intro, "Intro_Continue");
	}
}

void TStateIntro::Update(float DeltaTime)
{
	if (m_TimePassed < 5.0f)
	{
		// Less than five seconds.
		m_TimePassed += DeltaTime;
		m_IntroTextureRect.h += 48.0f * DeltaTime;
	}
}

void TStateIntro::Draw()
{
	Tilc::TWindow* Window = Tilc::GameObject->GetContext()->m_Window;
	SDL_Renderer* Renderer = Window->GetRenderer();
	SDL_RenderTexture(Renderer, m_IntroTexture, nullptr, &m_IntroTextureRect);

	if (Font && m_TimePassed >= 5.0f)
	{
		Font->DrawString(Renderer, 200.0f, 500.0f, "Press SPACE to continue");
	}
}

void TStateIntro::Continue(Tilc::TEventDetails* Details)
{
	if (m_TimePassed >= 5.0f)
	{
		m_StateManager->SwitchTo(Tilc::EStateType::MainMenu);
		m_StateManager->Remove(Tilc::EStateType::Intro);
	}
}
