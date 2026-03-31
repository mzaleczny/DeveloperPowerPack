#include "Tilc/Level.h"
#include "Tilc/Game.h"
#include "Tilc/StateManager.h"
#include "SDL3_image/SDL_image.h"

Tilc::TLevel::TLevel(Tilc::TBaseState* ParentState)
	: m_ParentState(ParentState)
{
}

Tilc::TLevel::~TLevel()
{
}
