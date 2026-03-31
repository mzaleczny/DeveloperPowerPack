#include "Tilc/Gui/Font.h"
#include "Tilc/GameState.h"
#include "SDL3_image/SDL_image.h"

Tilc::TGameState::TGameState() : m_MusicVolume(30), m_EffectVolume(40), m_Lives(5), m_MusicOn(true), m_EffectsOn(true)
{
}

Tilc::TGameState::~TGameState()
{
}
