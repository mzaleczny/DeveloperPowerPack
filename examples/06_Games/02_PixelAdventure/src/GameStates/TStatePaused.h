#pragma once
#include "Tilc/Tilc.h"
#include "Tilc/StateManager.h"
#include "Tilc/Game.h"
#include "SDL3/SDL.h"

struct SDL_Texture;
namespace Tilc::Gui
{
	class TTextBox;
}

class TStatePaused : public Tilc::TBaseState
{
public:
	TStatePaused(Tilc::TStateManager* StateManager);
	virtual void OnCreate() override;
	virtual void OnDestroy() override;
	virtual void Activate() override {};
	virtual void Deactivate() override {};
	virtual void Update(float DeltaTime) override {}; // DeltaTime czas w sekundach
	virtual void Draw() override;
	void Unpause(Tilc::TEventDetails* Details);

private:
	Tilc::Gui::TTextBox* TextBox = nullptr;
	SDL_Texture* TextTexture = nullptr;
	SDL_FRect TextRect;
};
