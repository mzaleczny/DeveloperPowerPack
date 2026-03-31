#include "StateMainMenu.h"
#include "Tilc/EventManager.h"
#include "Tilc/Localization.h"
#include "Tilc/Window.h"
#include "Tilc/Game.h"
#include "Tilc/Gui/Menu.h"
#include "Tilc/Gui/Font.h"
#include "Tilc/Utils/ExtString.h"

Tilc::TStateMainMenu::TStateMainMenu(Tilc::TStateManager* StateManager) : Tilc::TBaseState(StateManager)
{
}

void Tilc::TStateMainMenu::OnCreate()
{
	//m_Text.setString(_L("Menu główne"));

	if (Tilc::GameObject)
	{
        m_Menu = new Tilc::Gui::TMenu(nullptr, "GameMenu", {100.0f, 100.0f, 300.0f, 300.0f}, Tilc::GameObject->GetFont("MainFont"), 18.0f);
		if (m_Menu)
		{
			Tilc::TExtString Utf8String;
			m_Menu->AddItem("Nowa gra");
			m_Menu->AddItem(Utf8String.AnsiToUtf8("Zasługi"));
			m_Menu->AddItem(Utf8String.AnsiToUtf8("Opuść grę"));
			m_Menu->SetPosition(100.0f, 100.0f);
		}
	}

	TEventManager* evMgr = Tilc::GameObject->GetContext()->m_EventManager;
	if (evMgr)
	{
		evMgr->AddCallback(Tilc::EStateType::MainMenu, "Mouse_Left", &Tilc::TStateMainMenu::MouseClick, this);
		evMgr->AddCallback(Tilc::EStateType::MainMenu, "Key_Escape", &Tilc::TStateMainMenu::Close, this);
	}
}

void Tilc::TStateMainMenu::OnDestroy()
{
	Tilc::TEventManager* evMgr = Tilc::GameObject->GetContext()->m_EventManager;
	if (evMgr)
	{
		evMgr->RemoveCallback(Tilc::EStateType::MainMenu, "Mouse_Left");
	}
}

void Tilc::TStateMainMenu::Activate()
{
	if (m_StateManager->HasState(Tilc::EStateType::Game))
	{
	}
}

void Tilc::TStateMainMenu::Draw()
{
	SDL_Renderer* Renderer = Tilc::GameObject->GetContext()->m_Window->GetRenderer();
	m_Menu->Draw();
}

void Tilc::TStateMainMenu::MouseClick(Tilc::TEventDetails* Details)
{
	m_StateManager->SwitchTo(Tilc::EStateType::Game);
}


void Tilc::TStateMainMenu::Close(Tilc::TEventDetails* Details)
{
	Tilc::GameObject->GetContext()->m_Window->Close();
}
