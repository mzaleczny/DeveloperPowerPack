#include "Game.h"
#include "Tilc/Window.h"
#include "Tilc/Gui/Textbox.h"
#include "Tilc/StateManager.h"
#include "GameStates/TStateGame.h"

TGame::TGame(Tilc::EGameType GameType)
	: Tilc::TGame(GameType)
{
}

TGame::~TGame()
{
}

void TGame::OnInitialize()
{
    Tilc::TGame::OnInitialize();
    Tilc::GameObject->GetContext()->m_StateManager.RegisterState<TStateGame>(Tilc::EStateType::Game);
}
