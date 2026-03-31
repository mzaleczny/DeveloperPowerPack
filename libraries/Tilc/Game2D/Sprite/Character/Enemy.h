#pragma once
#include "Tilc/Game2D/Sprite/Character/Character.h"
#include "Tilc/Utils/Vector.h"
namespace Tilc
{
	class TEventDetails;
}

namespace Tilc::Game2D::Sprite
{
	class TEnemy : public TCharacter
	{
	public:
		TEnemy();
		~TEnemy();
		void OnEntityCollision(TEntityBase* Collider, bool Attack);
		virtual void Update(float DeltaTime) override;
	private:
		Tilc::TVector2D<float> m_Destination;
		float m_DestinationAcceptanceRadius;
		bool m_HasDestination;
	};
}
