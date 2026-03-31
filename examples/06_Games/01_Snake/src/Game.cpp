#include "Game.h"
#include "Tilc/Window.h"
#include "Tilc/Gui/Textbox.h"
#include "Tilc/StateManager.h"
#include "GameStates/TStateIntro.h"
#include "GameStates/TStateMainMenu.h"
#include "GameStates/TStateGame.h"
#include "GameStates/TStatePaused.h"

TGame::TGame(Tilc::EGameType GameType)
	: Tilc::TGame(GameType)
{
}

TGame::~TGame()
{
}

void TGame::OnInitialize()
{
	TStateMainMenu* MainMenuState;
	Tilc::GameObject->GetContext()->m_StateManager.RegisterState<TStateIntro>(Tilc::EStateType::Intro);
	Tilc::GameObject->GetContext()->m_StateManager.RegisterState<TStateMainMenu>(Tilc::EStateType::MainMenu);
	Tilc::GameObject->GetContext()->m_StateManager.RegisterState<TStateGame>(Tilc::EStateType::Game);
	Tilc::GameObject->GetContext()->m_StateManager.RegisterState<TStatePaused>(Tilc::EStateType::Paused);

	AddFont("Tuffy", "assets/fonts/tuffy.ttf", 16);
	AddFont("Arial", "assets/fonts/arial.ttf", 16);
}
