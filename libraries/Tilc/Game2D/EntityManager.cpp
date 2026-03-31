#include "Tilc/Game.h"
#include "Tilc/Game2D/EntityManager.h"
#include "Tilc/Game2D/World/MapLayer.h"
#include "Tilc/Game2D/Sprite/Character/Player.h"
#include "Tilc/Game2D/Sprite/Character/Enemy.h"
#include "Tilc/Utils/FileUtils.h"
#include "Tilc/Utils/FileUtils.h"
#include <sstream>

Tilc::Game2D::TEntityManager::TEntityManager(unsigned int MaxEntities)
	: m_MaxEntities(MaxEntities), m_IdCounter(1)
{
	if (m_MaxEntities > 0)
	{
		if (Tilc::FileExists("EnemyList.list"))
		{
			LoadEnemyTypes("EnemyList.list");
			m_EnemiesLoaded = true;
		}
		RegisterEntity<Tilc::Game2D::Sprite::TPlayer>(EEntityType::Player);
		RegisterEntity<Tilc::Game2D::Sprite::TEnemy>(EEntityType::Enemy);
	}
	if (Tilc::GameObject)
	{
		m_Context = Tilc::GameObject->GetContext();
	}
}

Tilc::Game2D::TEntityManager::~TEntityManager()
{
	Purge();
}

int Tilc::Game2D::TEntityManager::Add(const EEntityType Type, const Tilc::TExtString& Name)
{
	auto Iter = m_EntityFactory.find(Type);
	if (Iter == m_EntityFactory.end())
	{
		return -1;
	}

	TEntityBase* Entity = Iter->second();
	Entity->m_Id = m_IdCounter;
	if (Name != "")
	{
		Entity->m_Name = Name;
	}
	m_Entities.emplace(m_IdCounter, Entity);
	if (Type == EEntityType::Enemy)
	{
		auto Iter = m_EnemyTypes.find(Name);
		if (Iter != m_EnemyTypes.end())
		{
			Tilc::Game2D::Sprite::TEnemy* Enemy = (Tilc::Game2D::Sprite::TEnemy*)Entity;
			Enemy->Load(Iter->second);
		}
	}
	++m_IdCounter;
	return m_IdCounter - 1;
}

Tilc::Game2D::TEntityBase* Tilc::Game2D::TEntityManager::Find(unsigned int Id)
{
	auto Iter = m_Entities.find(Id);
	if (Iter == m_Entities.end())
	{
		return nullptr;
	}
	return Iter->second;
}

Tilc::Game2D::TEntityBase* Tilc::Game2D::TEntityManager::Find(const Tilc::TExtString& Name)
{
	for (auto& Iter : m_Entities)
	{
		if (Iter.second->m_Name == Name)
		{
			return Iter.second;
		}
	}
	return nullptr;
}

void Tilc::Game2D::TEntityManager::Remove(unsigned int Id)
{
	m_EntitiesToRemove.emplace_back(Id);
}

void Tilc::Game2D::TEntityManager::Update(float DeltaTime)
{
	for (auto& Iter : m_Entities)
	{
		Iter.second->Update(DeltaTime);
	}
	EntityCollisionCheck();
	ProcessRemovals();
}

void Tilc::Game2D::TEntityManager::Draw(SDL_Surface* Destination)
{
	for (auto& Iter : m_Entities)
	{
		// Draw Entity if it is inside view space
		if (Iter.second->m_Position.x >= Iter.second->ReferenceLayer->m_PosX &&
			Iter.second->m_Position.x < Iter.second->ReferenceLayer->m_PosX + m_Context->m_Window->GetWindowWidth() &&
			Iter.second->m_Position.y >= Iter.second->ReferenceLayer->m_PosY &&
			Iter.second->m_Position.y < Iter.second->ReferenceLayer->m_PosY + m_Context->m_Window->GetWindowHeight()
			)
		{
			Iter.second->Draw(Destination);
		}
	}
}

void Tilc::Game2D::TEntityManager::Purge()
{
	for (auto& Iter : m_Entities)
	{
		delete Iter.second;
	}
	m_Entities.clear();
	m_IdCounter = 1;
}

void Tilc::Game2D::TEntityManager::ProcessRemovals()
{
	while (m_EntitiesToRemove.begin() != m_EntitiesToRemove.end())
	{
		unsigned int Id = m_EntitiesToRemove.back();
		auto Iter = m_Entities.find(Id);
		if (Iter != m_Entities.end())
		{
			delete Iter->second;
			m_Entities.erase(Iter);
		}
		m_EntitiesToRemove.pop_back();
	}
}

void Tilc::Game2D::TEntityManager::LoadEnemyTypes(const Tilc::TExtString& FileName)
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
			std::string Name;
			std::string CharFile;
			KeyStream >> Name >> CharFile;
			m_EnemyTypes.emplace(Name, CharFile);
		}
	}
}

void Tilc::Game2D::TEntityManager::EntityCollisionCheck()
{
	if (m_Entities.empty())
	{
		return;
	}

	for (auto Iter = m_Entities.begin(); std::next(Iter) != m_Entities.end(); ++Iter)
	{
		for (auto Iter2 = std::next(Iter); Iter2 != m_Entities.end(); ++Iter2)
		{
			if (Iter->first == Iter2->first)
			{
				continue;
			}

			// Regular bounding box collision.
			if (SDL_HasRectIntersectionFloat(&Iter->second->m_BoundingBox, &Iter2->second->m_BoundingBox))
			{
				// boolean in the second arguments means if it is regular (false) collision or attack (true) collision
				Iter->second->OnEntityCollision(Iter2->second, false);
				Iter2->second->OnEntityCollision(Iter->second, false);

			}

			EEntityType EntityType1 = Iter->second->m_Type;
			EEntityType EntityType2 = Iter2->second->m_Type;
			if (EntityType1 == EEntityType::Player || EntityType1 == EEntityType::Enemy)
			{
				Tilc::Game2D::Sprite::TCharacter* Character1 = (Tilc::Game2D::Sprite::TCharacter*)Iter->second;
				if (SDL_HasRectIntersectionFloat(&Character1->m_AttackBoundingBox, &Iter2->second->m_BoundingBox))
				{
					// boolean in the second arguments means if it is regular (false) collision or attack (true) collision
					Character1->OnEntityCollision(Iter2->second, true);
				}
			}
			if (EntityType2 == EEntityType::Player || EntityType2 == EEntityType::Enemy)
			{
				Tilc::Game2D::Sprite::TCharacter* Character2 = (Tilc::Game2D::Sprite::TCharacter*)Iter2->second;
				if (SDL_HasRectIntersectionFloat(&Character2->m_AttackBoundingBox, &Iter->second->m_BoundingBox))
				{
					// boolean in the second arguments means if it is regular (false) collision or attack (true) collision
					Character2->OnEntityCollision(Iter->second, true);
				}
			}
		}
	}
}
