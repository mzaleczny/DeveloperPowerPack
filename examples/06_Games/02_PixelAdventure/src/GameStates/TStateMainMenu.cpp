#include "Tilc/Tilc.h"
#include "TStateMainMenu.h"
#include "Tilc/Game.h"

TStateMainMenu::TStateMainMenu(Tilc::TStateManager* StateManager) : Tilc::TStateMainMenu(StateManager)
{
}

void TStateMainMenu::OnCreate()
{
	if (Tilc::GameObject)
	{
		m_Font = new Tilc::Gui::TFont(Tilc::GameObject->GetFont("Tuffy"));
		if (Tilc::GameObject->GetContext()->m_TextureManager->RequireResource("StartScreen"))
		{
			m_Bg = (SDL_Surface*)Tilc::GameObject->GetContext()->m_TextureManager->GetResource("StartScreen")->m_Resource;
		}
	}

	m_EventManager = Tilc::GameObject->GetContext()->m_EventManager;
	if (m_EventManager)
	{
		m_EventManager->AddCallback(Tilc::EStateType::MainMenu, "Key_Escape", &TStateMainMenu::ExitGame, this);
	}

	m_Window = Tilc::GameObject->GetContext()->m_Window;
}

void TStateMainMenu::OnDestroy()
{
	if (m_Font)
	{
		delete m_Font;
		m_Font = nullptr;
	}
	if (Tilc::GameObject->GetContext()->m_TextureManager && m_Bg)
	{
		Tilc::GameObject->GetContext()->m_TextureManager->ReleaseResource("MainMenu");
		m_Bg = nullptr;
	}
	if (m_EventManager)
	{
		m_EventManager->RemoveCallback(Tilc::EStateType::MainMenu, "Key_Escape");
	}
}

void TStateMainMenu::Update(float DeltaTime)
{
}

void TStateMainMenu::Draw()
{
	SDL_BlitSurface(m_Bg, nullptr, m_Window->GetRenderSurface(), nullptr);
	m_Font->DrawString(m_Window->GetRenderSurface(), 200.0f, 500.0f, "Press SPACE to continue");
}

void TStateMainMenu::ExitGame(Tilc::TEventDetails* Details)
{
	m_Window->Close(Details);
}
