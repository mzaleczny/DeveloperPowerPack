#pragma once

#include "Tilc/Tilc.h"
#include "Tilc/StateManager.h"

namespace Tilc::Gui
{
	class TTextBox;
	class TStyledWindow;
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

	void KeyEscapePressed(Tilc::TEventDetails* Details);

protected:
    Tilc::Gui::TTextBox* m_TextBox{};
	float m_Elapsed;
};
