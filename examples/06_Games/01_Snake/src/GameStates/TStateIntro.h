#pragma once

#include "Tilc/StateManager.h"
#include "Tilc/EventManager.h"
#include "Tilc/Game.h"
#include <SDL3/SDL.h>

struct SDL_Texture;

namespace Tilc::Gui
{
	class TFont;
}

class TStateIntro : public Tilc::TBaseState
{
public:
	TStateIntro(Tilc::TStateManager* StateManager);
	virtual void OnCreate() override;
	virtual void OnDestroy() override;
	virtual void Activate() override {};
	virtual void Deactivate() override {};
	virtual void Update(float DeltaTime) override; // DeltaTime czas w sekundach
	virtual void Draw() override;
	void Continue(Tilc::TEventDetails* Details);

private:
	SDL_Texture* m_IntroTexture;
	Tilc::Gui::TFont* Font;
	SDL_FRect m_IntroTextureRect;

	float m_TimePassed;
	Tilc::TEventManager* evMgr = nullptr;
};
