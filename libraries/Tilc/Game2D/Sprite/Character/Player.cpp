#include "Tilc/Game2D/Sprite/Character/Player.h"
#include "Tilc/Game2D/Sprite/BaseAnimation.h"
#include "Tilc/Utils/ExtString.h"
#include "Tilc/Utils/Vector.h"
#include "Tilc/EventManager.h"
#include "Tilc/Game.h"

Tilc::Game2D::Sprite::TPlayer::TPlayer()
{
	Load("data/Player.char");
	m_Type = EEntityType::Player;
	TEventManager* EventManager = GameObject->GetContext()->m_EventManager;
	if (EventManager)
	{
		EventManager->AddCallback<TPlayer>(EStateType::Game, "Player_MoveLeft", &TPlayer::InputHandler, this);
		EventManager->AddCallback<TPlayer>(EStateType::Game, "Player_MoveRight", &TPlayer::InputHandler, this);
		EventManager->AddCallback<TPlayer>(EStateType::Game, "Player_Jump", &TPlayer::InputHandler, this);
		EventManager->AddCallback<TPlayer>(EStateType::Game, "Player_Attack", &TPlayer::InputHandler, this);
	}
}

Tilc::Game2D::Sprite::TPlayer::~TPlayer()
{
	TEventManager* EventManager = GameObject->GetContext()->m_EventManager;
	if (EventManager)
	{
		EventManager->RemoveCallback(EStateType::Game, "Player_MoveLeft");
		EventManager->RemoveCallback(EStateType::Game, "Player_MoveRight");
		EventManager->RemoveCallback(EStateType::Game, "Player_Jump");
		EventManager->RemoveCallback(EStateType::Game, "Player_Attack");
	}
}

void Tilc::Game2D::Sprite::TPlayer::OnEntityCollision(TEntityBase* Collider, bool Attack)
{
	if (m_State == EEntityState::Die)
	{
		return;
	}

	if (Attack)
	{
		if (m_State != EEntityState::Attack)
		{
			return;
		}

		if (!m_SpriteSheet.GetCurrentAnimation()->IsInAction())
		{
			return;
		}

		if (Collider->GetType() != EEntityType::Enemy && Collider->GetType() != EEntityType::Player)
		{
			return;
		}

		TCharacter* Opponent = dynamic_cast<TCharacter*>(Collider);
		Opponent->GetDamage(m_Damage);
		if (m_Position.x > Opponent->GetPosition().x)
		{
			Opponent->AddVelocity(-32, 0);
		}
		else
		{
			Opponent->AddVelocity(32, 0);
		}
	}
	else {
		// Other behavior.
	}
}

void Tilc::Game2D::Sprite::TPlayer::InputHandler(TEventDetails* Details)
{
}

