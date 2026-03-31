#include "GameStates/TStateGame.h"
#include "Tilc/StateManager.h"
#include "Tilc/Window.h"
#include "Tilc/Game.h"
#include "Tilc/Gui/TextBox.h"

TStateGame::TStateGame(Tilc::TStateManager* StateManager) : Tilc::TBaseState(StateManager),
	m_World(Tilc::GameObject->GetContext()->m_Window->GetWindowSize()), m_Snake(m_World.GetBlockSize())
{
	Tilc::TExtString s("Seeded random number generator with: " + std::to_string(time(NULL)));
	m_TextBox = new Tilc::Gui::TTextBox(0.0f, 0.0f, s.c_str(), s.length(), Tilc::GameObject->GetFont("Tuffy"), 14.0f);
}

TStateGame::~TStateGame()
{
	if (m_TextBox)
	{
		delete m_TextBox;
		m_TextBox = nullptr;
	}
}

void TStateGame::OnCreate()
{
	Tilc::TEventManager* evMgr = Tilc::GameObject->GetContext()->m_EventManager;
	if (evMgr)
	{
		evMgr->AddCallback(Tilc::EStateType::Game, "Key_Escape", &TStateGame::ExitToMainMenu, this);
		evMgr->AddCallback(Tilc::EStateType::Game, "Key_P", &TStateGame::Pause, this);
		evMgr->AddCallback(Tilc::EStateType::Game, "Key_Up", &TStateGame::KeyUpPressed, this);
		evMgr->AddCallback(Tilc::EStateType::Game, "Key_Down", &TStateGame::KeyDownPressed, this);
		evMgr->AddCallback(Tilc::EStateType::Game, "Key_Left", &TStateGame::KeyLeftPressed, this);
		evMgr->AddCallback(Tilc::EStateType::Game, "Key_Right", &TStateGame::KeyRightPressed, this);
	}
	m_Elapsed = 0.0f;
}

void TStateGame::OnDestroy()
{
	Tilc::TEventManager* evMgr = Tilc::GameObject->GetContext()->m_EventManager;
	if (evMgr)
	{
		evMgr->RemoveCallback(Tilc::EStateType::Game, "Key_Right");
		evMgr->RemoveCallback(Tilc::EStateType::Game, "Key_Left");
		evMgr->RemoveCallback(Tilc::EStateType::Game, "Key_Down");
		evMgr->RemoveCallback(Tilc::EStateType::Game, "Key_Up");
		evMgr->RemoveCallback(Tilc::EStateType::Game, "Key_P");
		evMgr->RemoveCallback(Tilc::EStateType::Game, "Key_Escape");
	}
}

void TStateGame::Update(float DeltaTime)
{
	static Tilc::TWindow* Window = Tilc::GameObject->GetContext()->m_Window;

	float TimeStep = 1.0f / m_Snake.GetSpeed();
	m_Elapsed += DeltaTime;
	if (m_Elapsed >= TimeStep)
	{
		m_Snake.Tick();
		m_World.Update(m_Snake);
		m_Elapsed -= TimeStep;
		if (m_Snake.HasLost())
		{
			m_Snake.Reset();
		}
	}
}

void TStateGame::Draw()
{
	static Tilc::TWindow* Window = Tilc::GameObject->GetContext()->m_Window;
	static SDL_Renderer* Renderer = Window->GetRenderer();

	m_World.Render(Renderer);
	m_Snake.Render(Renderer);
	m_TextBox->Draw(Renderer);
}

void TStateGame::ExitToMainMenu(Tilc::TEventDetails* Details)
{
	m_StateManager->SwitchTo(Tilc::EStateType::MainMenu);
}

void TStateGame::Pause(Tilc::TEventDetails* Details)
{
	m_StateManager->SwitchTo(Tilc::EStateType::Paused);
}

void TStateGame::KeyUpPressed(Tilc::TEventDetails* Details)
{
	if (m_Snake.GetPhysicalDirection() != EDirection::Down)
	{
		m_Snake.SetDirection(EDirection::Up);
	}
}

void TStateGame::KeyDownPressed(Tilc::TEventDetails* Details)
{
	if (m_Snake.GetPhysicalDirection() != EDirection::Up)
	{
		m_Snake.SetDirection(EDirection::Down);
	}
}

void TStateGame::KeyLeftPressed(Tilc::TEventDetails* Details)
{
	if (m_Snake.GetPhysicalDirection() != EDirection::Right)
	{
		m_Snake.SetDirection(EDirection::Left);
	}
}

void TStateGame::KeyRightPressed(Tilc::TEventDetails* Details)
{
	if (m_Snake.GetPhysicalDirection() != EDirection::Left)
	{
		m_Snake.SetDirection(EDirection::Right);
	}
}

