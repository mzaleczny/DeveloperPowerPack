#include "Tilc/Game2D/Sprite/Character/Character.h"
#include "Tilc/Game2D/Sprite/BaseAnimation.h"
#include "Tilc/Game2D/EntityManager.h"
#include "Tilc/Utils/FileUtils.h"
#include "Tilc/Game.h"
#include <string>
#include <sstream>
#include <math.h>

Tilc::Game2D::Sprite::TCharacter::TCharacter()
	: m_JumpVelocity(250.0f), m_HitPoints(10), m_Damage(2.0f)
{
	m_Name = "Character";
}

Tilc::Game2D::Sprite::TCharacter::~TCharacter()
{
}

void Tilc::Game2D::Sprite::TCharacter::SetState(EEntityState State)
{
	TEntityBase::SetState(State);
	if (m_State != EEntityState::Jump)
	{
		// Reset available jump amounts
		m_AvailableJumpsLeft = m_MaxAvailableJumpsLeft;
	}
}

void Tilc::Game2D::Sprite::TCharacter::Move(const EDirection& Direction)
{
	if (m_State == EEntityState::Die)
	{
		return;
	}

	m_SpriteSheet.SetDirection(Direction);

	if (Direction == EDirection::Left)
	{
		Accelerate(-m_Speed.x, 0);
	}
	else
	{
		Accelerate(m_Speed.x, 0);
	}

	if (m_State == EEntityState::Idle)
	{
		SetState(EEntityState::Walk);
	}
}

void Tilc::Game2D::Sprite::TCharacter::Jump()
{
	if (m_AvailableJumpsLeft < 1 || m_State == EEntityState::Die || m_State == EEntityState::Hurt)
	{
		return;
	}

	--m_AvailableJumpsLeft;
	SetState(EEntityState::Jump);
	AddVelocity(0, -m_JumpVelocity);
}

void Tilc::Game2D::Sprite::TCharacter::Attack()
{
	if (m_State == EEntityState::Die || m_State == EEntityState::Jump || m_State == EEntityState::Hurt || m_State == EEntityState::Attack)
	{
		return;
	}
	SetState(EEntityState::Attack);
}

void Tilc::Game2D::Sprite::TCharacter::GetDamage(const int Damage)
{
	if (m_State == EEntityState::Die || m_State == EEntityState::Hurt)
	{
		return;
	}
	m_HitPoints = (m_HitPoints - Damage > 0 ? m_HitPoints - Damage : 0);
	if (m_HitPoints)
	{
		SetState(EEntityState::Hurt);
	}
	else
	{
		SetState(EEntityState::Die);
	}
}

void Tilc::Game2D::Sprite::TCharacter::Load(const Tilc::TExtString& FileName)
{
	Tilc::TExtString Line;
	int LineNumber = 0;
	Tilc::TCFile File(FileName.c_str());
	if (File.m_File)
	{
		while (File.ReadLine(Line))
		{
			++LineNumber;
			if (Line[0] == '#')
			{
				continue;
			}
			std::stringstream KeyStream(Line);
			std::string Type;
			KeyStream >> Type;
			if (Type == "Name")
			{
				KeyStream >> m_Name;
			}
			else if (Type == "Spritesheet")
			{
				std::string Path;
				KeyStream >> Path;
				m_SpriteSheet.LoadSheet(Path);
			}
			else if (Type == "Hitpoints")
			{
				KeyStream >> m_HitPoints;
			}
			else if (Type == "BoundingBox")
			{
				float BoundingBoxWidth;
				float BoundingBoxHeight;
				KeyStream >> BoundingBoxWidth >> BoundingBoxHeight;
				SetSize(BoundingBoxWidth, BoundingBoxHeight);
			}
			else if (Type == "DamageBox")
			{
				KeyStream >> m_AttackBoundingBoxOffset.x >> m_AttackBoundingBoxOffset.y >> m_AttackBoundingBox.w >> m_AttackBoundingBox.h;
			}
			else if (Type == "Speed")
			{
				KeyStream >> m_Speed.x >> m_Speed.y;
			}
			else if (Type == "JumpVelocity")
			{
				KeyStream >> m_JumpVelocity;
			}
			else if (Type == "MaxVelocity")
			{
				KeyStream >> m_MaxVelocity.x >> m_MaxVelocity.y;
			}
			else if (Type == "Damage")
			{
				KeyStream >> m_Damage;
			}
			else
			{
				SDL_Log("ERROR! Unknown type '%s' in character file %s in line %d\n", Type.c_str(), FileName.c_str(), LineNumber);
			}
		}

	}
}

void Tilc::Game2D::Sprite::TCharacter::Update(float DeltaTime)
{
	TEntityBase::Update(DeltaTime);

	if (m_AttackBoundingBox.w != 0 && m_AttackBoundingBox.h != 0)
	{
		UpdateAttackBoundingBox();
	}

	if (m_State != EEntityState::Die && m_State != EEntityState::Attack && m_State != EEntityState::Hurt)
	{
		if (fabs(m_Velocity.y) >= 0.001f)
		{
			SetState(EEntityState::Jump);
		}
		else if (fabs(m_Velocity.x) >= 0.1f)
		{
			SetState(EEntityState::Walk);
		}
		else
		{
			SetState(EEntityState::Idle);
		}
	}
	else if (m_State == EEntityState::Attack || m_State == EEntityState::Hurt)
	{
		if (!m_SpriteSheet.GetCurrentAnimation()->IsPlaying())
		{
			SetState(EEntityState::Idle);
		}
	}
	else if (m_State == EEntityState::Die)
	{
		if (!m_SpriteSheet.GetCurrentAnimation()->IsPlaying())
		{
			m_EntityManager->Remove(m_Id);
		}
	}
	Animate();
	m_SpriteSheet.Update(DeltaTime);
	//m_SpriteSheet.SetSpritePosition(m_Position);
}

void Tilc::Game2D::Sprite::TCharacter::UpdateAttackBoundingBox()
{
	m_AttackBoundingBox.x = (m_SpriteSheet.GetDirection() == EDirection::Left ? (m_BoundingBox.x - m_AttackBoundingBox.w) - m_AttackBoundingBoxOffset.x
			: (m_BoundingBox.x + m_BoundingBox.w) + m_AttackBoundingBoxOffset.x);
	m_AttackBoundingBox.y = m_BoundingBox.y + m_AttackBoundingBoxOffset.y;
}

void Tilc::Game2D::Sprite::TCharacter::Animate()
{
	EEntityState State = m_State;
	if (State == EEntityState::Walk && m_SpriteSheet.GetCurrentAnimation()->GetName() != "Walk")
	{
		m_SpriteSheet.SetAnimation("Walk", true, true);
	}
	else if (State == EEntityState::Jump && m_SpriteSheet.GetCurrentAnimation()->GetName() != "Jump")
	{
		m_SpriteSheet.SetAnimation("Jump", true, false);
	}
	else if (State == EEntityState::Attack && m_SpriteSheet.GetCurrentAnimation()->GetName() != "Attack")
	{
		m_SpriteSheet.SetAnimation("Attack", true, false);
	}
	else if (State == EEntityState::Hurt && m_SpriteSheet.GetCurrentAnimation()->GetName() != "Hurt")
	{
		m_SpriteSheet.SetAnimation("Hurt", true, false);
	}
	else if (State == EEntityState::Die && m_SpriteSheet.GetCurrentAnimation()->GetName() != "Death")
	{
		m_SpriteSheet.SetAnimation("Death", true, false);
	}
	else if (State == EEntityState::Idle && m_SpriteSheet.GetCurrentAnimation()->GetName() != "Idle")
	{
		m_SpriteSheet.SetAnimation("Idle", true, true);
	}
}
