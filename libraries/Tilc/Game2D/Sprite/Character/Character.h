#pragma once
#include "Tilc/Game2D/Sprite/CommonTypes.h"
#include "Tilc/Game2D/Entity.h"
#include "Tilc/Game2D/Sprite/SpriteSheet.h"
#include "Tilc/Utils/ExtString.h"
#include "Tilc/Utils/Vector.h"
#include "SDL3/SDL.h"

namespace Tilc::Game2D::Sprite
{
	class TCharacter : public TEntityBase
	{
		friend class TEntityManager;
	public:
		TCharacter();
		virtual ~TCharacter();
		virtual void SetState(EEntityState State) override;
		void Move(const EDirection& Direction);
		void Jump();
		void Attack();
		void GetDamage(const int Damage);
		void Load(const Tilc::TExtString& FileName);
		virtual void OnEntityCollision(TEntityBase* Collider, bool CheckAttackBoundingRect) = 0;
		virtual void Update(float DeltaTime);
		inline void Draw(SDL_Surface* Destination)
		{
			m_SpriteSheet.Draw(Destination);
		}
	protected:
		void UpdateAttackBoundingBox();
		void Animate();
		TSpriteSheet m_SpriteSheet;
		float m_JumpVelocity;
		float m_Damage; // Damage inflicting
		int m_HitPoints;
		int m_MaxAvailableJumpsLeft = 1;
		int m_AvailableJumpsLeft = 1;
		SDL_FRect m_AttackBoundingBox;
		Tilc::TVector2D<float> m_AttackBoundingBoxOffset;
	};
}
