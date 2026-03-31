#include "Tilc/Game.h"
#include "Tilc/Game2D/Entity.h"
#include "Tilc/Game2D/World/World.h"
#include "Tilc/Game2D/World/MapLayer.h"
#include <math.h>

Tilc::Game2D::TEntityBase::TEntityBase()
	: m_Name("BaseEntity"), m_Type(EEntityType::Base), m_GroundTile(nullptr), m_State(EEntityState::Idle), m_Id(0),
		m_CollidingOnX(false), m_CollidingOnY(false), m_Friction {0.8f, 1.0f}
{
	// By default we take first layer map
	int ReferenceLayerIndex = 0;
	if (Tilc::GameObject)
	{
		m_EntityManager = Tilc::GameObject->GetContext()->m_EntityManager;
		World = Tilc::GameObject->GetContext()->m_World2D;
		ReferenceLayer = World->m_Layers[ReferenceLayerIndex];
	}
}

Tilc::Game2D::TEntityBase::~TEntityBase()
{
}

void Tilc::Game2D::TEntityBase::SetState(EEntityState State)
{
	if (m_State == EEntityState::Die)
	{
		return;
	}
	m_State = State;
}

void Tilc::Game2D::TEntityBase::Move(float dx, float dy)
{
	m_PositionOld = m_Position;
	m_Position.x += dx;
	m_Position.x += dy;
	
	Tilc::TVector2D<int> MapSize{ ReferenceLayer->m_MapWidth, ReferenceLayer->m_MapHeight };
	if (m_Position.x < 0)
	{
		m_Position.x = 0;
	}
	else if (m_Position.x > (MapSize.x + 1) * ReferenceLayer->m_TileWidth)
	{
		m_Position.x = (MapSize.x + 1) * ReferenceLayer->m_TileWidth;
	}
	if (m_Position.y < 0)
	{
		m_Position.y = 0;
	}
	else if (m_Position.y > (MapSize.y + 1) * ReferenceLayer->m_TileHeight)
	{
		m_Position.y = (MapSize.y + 1) * ReferenceLayer->m_TileHeight;
		SetState(EEntityState::Die);
	}
	UpdateBoundingBox();
}

void Tilc::Game2D::TEntityBase::AddVelocity(float dx, float dy)
{
	m_Velocity.x += dx;
	m_Velocity.y += dy;

	if (fabs(m_Velocity.x) > m_MaxVelocity.x)
	{
		if (m_Velocity.x < 0)
		{
			m_Velocity.x = -m_MaxVelocity.x;
		}
		else
		{
			m_Velocity.x = m_MaxVelocity.x;
		}
	}

	if (fabs(m_Velocity.y) > m_MaxVelocity.y)
	{
		if (m_Velocity.y < 0)
		{
			m_Velocity.y = -m_MaxVelocity.y;
		}
		else
		{
			m_Velocity.y = m_MaxVelocity.y;
		}
	}
}

void Tilc::Game2D::TEntityBase::ApplyFriction(float fx, float fy)
{
	if (m_Velocity.x != 0)
	{
		if (fabs(m_Velocity.x) - fabs(fx) < 0)
		{
			m_Velocity.x = 0;
		}
		else
		{
			if (m_Velocity.x < 0)
			{
				m_Velocity.x += fx;
			}
			else
			{
				m_Velocity.x -= fx;
			}
		}
	}

	if (m_Velocity.y != 0)
	{
		if (fabs(m_Velocity.y) - fabs(fy) < 0)
		{
			m_Velocity.y = 0;
		}
		else
		{
			if (m_Velocity.y < 0)
			{
				m_Velocity.y += fy;
			}
			else
			{
				m_Velocity.y -= fy;
			}
		}
	}
}

void Tilc::Game2D::TEntityBase::Update(float DeltaTime)
{
	float Gravity = World->GetGravity();
	Accelerate(0, Gravity);
	AddVelocity(m_Acceleration.x * DeltaTime, m_Acceleration.y * DeltaTime);
	SetAcceleration(0.0f, 0.0f);

	Tilc::TVector2D<float> FrictionValue;
	if (m_GroundTile)
	{
		FrictionValue = m_GroundTile->m_Friction;
		if (m_GroundTile->m_Deadly)
		{
			SetState(EEntityState::Die);
		}
	}
	else if (ReferenceLayer->m_DefaultTile)
	{
		FrictionValue = ReferenceLayer->m_DefaultTile->m_Friction;
	}
	else
	{
		FrictionValue = m_Friction;
	}

	float FrictionX = (m_Speed.x * FrictionValue.x) * DeltaTime;
	float FrictionY = (m_Speed.y * FrictionValue.y) * DeltaTime;
	ApplyFriction(FrictionX, FrictionY);
	Tilc::TVector2D<float> DeltaPos = m_Velocity * DeltaTime;
	Move(DeltaPos.x, DeltaPos.y);
	m_CollidingOnX = false;
	m_CollidingOnY = false;
	CheckCollisions();
	ResolveCollisions();
}

void Tilc::Game2D::TEntityBase::CheckCollisions()
{
	int FromX = floor(m_BoundingBox.x / ReferenceLayer->m_TileWidth);
	int ToX = floor((m_BoundingBox.x + m_BoundingBox.w) / ReferenceLayer->m_TileWidth);
	int FromY = floor(m_BoundingBox.y / ReferenceLayer->m_TileHeight);
	int ToY = floor((m_BoundingBox.y + m_BoundingBox.h) / ReferenceLayer->m_TileHeight);
	for (int x = FromX; x <= ToX; ++x)
	{
		for (int y = FromY; y <= ToY; ++y)
		{
			TTile* Tile = ReferenceLayer->m_TileMap[y * ReferenceLayer->m_MapWidth + x];
			if (!Tile)
			{
				continue;
			}
			SDL_FRect TileBounds{ (float)x * ReferenceLayer->m_TileWidth, (float)y * ReferenceLayer->m_TileHeight, (float)ReferenceLayer->m_TileWidth, (float)ReferenceLayer->m_TileHeight };
			SDL_FRect Intersection;
			SDL_GetRectIntersectionFloat(&m_BoundingBox, &TileBounds, &Intersection);
			float Area = Intersection.w * Intersection.h;
			TCollisionElement CollisionElement(Area, Tile->m_Properties, TileBounds);
			m_Collisions.emplace_back(CollisionElement);
			if (Tile->m_Warp && m_Type == EEntityType::Player)
			{
				World->LoadNext();
			}
		}
	}
}

void Tilc::Game2D::TEntityBase::ResolveCollisions()
{
	if (!m_Collisions.empty())
	{
		std::sort(m_Collisions.begin(), m_Collisions.end(), [](TCollisionElement& a, TCollisionElement& b) { return a.m_Area > b.m_Area; });
		for (auto& Iter : m_Collisions)
		{
			if (!SDL_HasRectIntersectionFloat(&m_BoundingBox, &Iter.m_TileBounds))
			{
				continue;
			}
			float XDiff = (m_BoundingBox.x + (m_BoundingBox.w / 2)) - (Iter.m_TileBounds.x + (Iter.m_TileBounds.w / 2));
			float YDiff = (m_BoundingBox.y + (m_BoundingBox.h / 2)) - (Iter.m_TileBounds.y + (Iter.m_TileBounds.h / 2));
			float Resolve = 0;
			if (fabs(XDiff) > fabs(YDiff))
			{
				if (XDiff > 0)
				{
					Resolve = (Iter.m_TileBounds.x + ReferenceLayer->m_TileWidth) - m_BoundingBox.x;
				}
				else
				{
					Resolve = -((m_BoundingBox.x + m_BoundingBox.w) - Iter.m_TileBounds.x);
				}
				Move(Resolve, 0);
				m_Velocity.x = 0;
				m_CollidingOnX = true;
			}
			else
			{
				if (YDiff > 0)
				{
					Resolve = (Iter.m_TileBounds.y + ReferenceLayer->m_TileHeight) - m_BoundingBox.y;
				}
				else
				{
					Resolve = -((m_BoundingBox.y + m_BoundingBox.h) - Iter.m_TileBounds.y);
				}
				Move(0, Resolve);
				m_Velocity.y = 0;
				if (m_CollidingOnY)
				{
					continue;
				}
				m_GroundTile = Iter.m_Tile;
				m_CollidingOnY = true;
			}
		}
		m_Collisions.clear();
	}
	if (!m_CollidingOnY)
	{
		m_GroundTile = nullptr;
	}
}
