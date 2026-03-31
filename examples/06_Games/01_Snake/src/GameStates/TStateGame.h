#pragma once

#include "Tilc/Tilc.h"
#include "Tilc/StateManager.h"
#include "World.h"
#include "Snake.h"

namespace Tilc::Gui
{
	class TTextBox;
}
class TStateGame : public Tilc::TBaseState
{
public:
	TStateGame(Tilc::TStateManager* StateManager);
	virtual ~TStateGame();
	virtual void OnCreate() override;
	virtual void OnDestroy() override;
	virtual void Activate() override {};
	virtual void Deactivate() override {};
	virtual void Update(float DeltaTime) override; // DeltaTime czas w sekundach
	virtual void Draw() override;

	void ExitToMainMenu(Tilc::TEventDetails* Details);
	void Pause(Tilc::TEventDetails* Details);
	void KeyUpPressed(Tilc::TEventDetails* Details);
	void KeyDownPressed(Tilc::TEventDetails* Details);
	void KeyLeftPressed(Tilc::TEventDetails* Details);
	void KeyRightPressed(Tilc::TEventDetails* Details);

protected:
	TWorld m_World;
	TSnake m_Snake;
	Tilc::Gui::TTextBox* m_TextBox;
	float m_Elapsed;
};
