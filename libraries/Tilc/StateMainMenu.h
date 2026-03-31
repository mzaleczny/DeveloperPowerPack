#pragma once
#include "Tilc/DllGlobals.h"
#include "Tilc/Utils/ExtString.h"
#include <vector>
#include <SDL3/SDL.h>
#include "Tilc/StateManager.h"
#include "Tilc/Gui/Menu.h"

namespace Tilc
{

	struct TEventDetails;

	class DECLSPEC TStateMainMenu : public TBaseState
	{
	public:
		TStateMainMenu(Tilc::TStateManager* StateManager);
		virtual void OnCreate() override;
		virtual void OnDestroy() override;
		virtual void Activate() override;
		virtual void Deactivate() override {}
		virtual void Update(float DeltaTime) override {}; // DeltaTime czas w sekundach
		virtual void Draw() override;
		void MouseClick(Tilc::TEventDetails* Details);
		void Close(Tilc::TEventDetails* Details);
		void SetMenuPosition(const float X, const float Y)
		{
			if (m_Menu)
			{
				m_Menu->SetPosition(X, Y);
			}
		}
	protected:
		Tilc::Gui::TMenu* m_Menu;
	};

}// namespace Tilc
