#include "Tilc/Tilc.h"
#include "TStateIntro.h"
#include <SDL3_image/SDL_image.h>

TStateIntro::TStateIntro(Tilc::TStateManager* StateManager) : Tilc::TBaseState(StateManager), m_TimePassed(0.0f)
{
}

void TStateIntro::OnCreate()
{
	if (Tilc::GameObject)
	{
		m_Font = new Tilc::Gui::TFont(Tilc::GameObject->GetFont("Tuffy"));
		if (Tilc::GameObject->GetContext()->m_TextureManager->RequireResource("StartScreen"))
		{
			m_IntroTexture = nullptr;// (SDL_Surface*)Tilc::GameObject->GetContext()->m_TextureManager->GetResource("StartScreen")->m_Resource;
		}
	}

	m_EventManager = Tilc::GameObject->GetContext()->m_EventManager;
	if (m_EventManager)
	{
		m_EventManager->AddCallback(Tilc::EStateType::Intro, "Intro_Continue", &TStateIntro::Continue, this);
	}

	m_Window = Tilc::GameObject->GetContext()->m_Window;
}

void TStateIntro::OnDestroy()
{
	if (m_Font)
	{
		delete m_Font;
		m_Font = nullptr;
	}
	if (Tilc::GameObject->GetContext()->m_TextureManager && m_IntroTexture)
	{
		Tilc::GameObject->GetContext()->m_TextureManager->ReleaseResource("StartScreen");
		m_IntroTexture = nullptr;
	}
	if (m_EventManager)
	{
		m_EventManager->RemoveCallback(Tilc::EStateType::Intro, "Intro_Continue");
	}
}

void TStateIntro::Update(float DeltaTime)
{
	m_TimePassed += DeltaTime;
}

void TStateIntro::Draw()
{
	SDL_BlitSurface(m_IntroTexture, nullptr, m_Window->GetRenderSurface(), nullptr);

	if (m_Font && m_TimePassed >= 2.0f)
	{
		m_Font->DrawString(m_Window->GetRenderSurface(), 200.0f, 500.0f, "Press SPACE to continue");
	}
}

void TStateIntro::Continue(Tilc::TEventDetails* Details)
{
	if (m_TimePassed >= 2.0f)
	{
		m_StateManager->SwitchTo(Tilc::EStateType::MainMenu);
		m_StateManager->Remove(Tilc::EStateType::Intro);
	}
}
