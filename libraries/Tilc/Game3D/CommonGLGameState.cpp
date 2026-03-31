#include "Tilc/Game3D/CommonGLGameState.h"
#include "Tilc/EventManager.h"
#include "Tilc/Localization.h"
#include "Tilc/Window.h"
#include "Tilc/Game.h"
#include "Tilc/Gui/Menu.h"
#include "Tilc/Gui/Font.h"
#include "Tilc/Utils/ExtString.h"

Tilc::TCommonGLGameState::TCommonGLGameState(Tilc::TStateManager* StateManager) : Tilc::TBaseState(StateManager)
{
}

Tilc::TCommonGLGameState::~TCommonGLGameState()
{
}
