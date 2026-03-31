#pragma once
#include "Tilc/Game2D/Sprite/Character/Character.h"

namespace Tilc
{
	class TEventDetails;
}

namespace Tilc::Game2D::Sprite
{
	class TPlayer : public TCharacter
	{
	public:
		TPlayer();
		~TPlayer();
		void OnEntityCollision(TEntityBase* Collider, bool Attack);
		void InputHandler(TEventDetails* Details);
	};
}
