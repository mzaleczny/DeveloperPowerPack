#include "Enemy.h"

Tilc::Game2D::Sprite::TEnemy::TEnemy()
	: m_HasDestination(false), m_DestinationAcceptanceRadius(16.0f)
{
	m_Type = EEntityType::Enemy;
}

Tilc::Game2D::Sprite::TEnemy::~TEnemy()
{
}

void Tilc::Game2D::Sprite::TEnemy::OnEntityCollision(TEntityBase* Collider, bool Attack)
{
	if (m_State == EEntityState::Die)
	{
		return;
	}
	
	if (Attack)
	{
		return;
	}

	if (Collider->GetType() != EEntityType::Player)
	{
		return;
	}

	TCharacter* Player = dynamic_cast<TCharacter*>(Collider);
	if (!Player)
	{
		return;
	}

	SetState(EEntityState::Attack);
	Player->GetDamage(m_Damage);
	if (m_Position.x > Player->GetPosition().x)
	{
		Player->AddVelocity(-m_Speed.x, 0);
		m_SpriteSheet.SetDirection(EDirection::Left);
	}
	else
	{
		Player->AddVelocity(m_Speed.y, 0);
		m_SpriteSheet.SetDirection(EDirection::Right);
	}
}

void Tilc::Game2D::Sprite::TEnemy::Update(float DeltaTime)
{
	TCharacter::Update(DeltaTime);
	if (m_HasDestination)
	{
		if (fabs(m_Destination.x - m_Position.x) < m_DestinationAcceptanceRadius)
		{
			m_HasDestination = false;
			return;
		}
		if (m_Destination.x - m_Position.x > 0)
		{
			Move(EDirection::Right);
		}
		else
		{
			Move(EDirection::Left);
		}
		if (m_CollidingOnX)
		{
			m_HasDestination = false;
		}
		return;
	}

	int Random = rand() % 1000 + 1;
	if (Random != 1000)
	{
		return;
	}
	int NewX = rand() % 65 + 0;
	if (rand() % 2)
	{
		NewX = -NewX;
	}
	m_Destination.x = m_Position.x + NewX;
	if (m_Destination.x < 0)
	{
		m_Destination.x = 0;
	}
	m_HasDestination = true;
}
