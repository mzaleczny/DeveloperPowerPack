#pragma once
#include "Tilc/DllGlobals.h"
#include "Tilc/Game2D/Entity.h"
#include "Tilc/Utils/ExtString.h"
#include "Tilc/Utils/VectorContainer.h"
#include <unordered_map>
#include <functional>
#include "SDL3/SDL.h"

namespace Tilc
{
    class TSharedContext;
}

namespace Tilc::Game2D
{
    using TEntityContainer = std::unordered_map<unsigned int, TEntityBase*>;
    using TEntityFactory = std::unordered_map<EEntityType, std::function<TEntityBase*(void)>>;
    // TEnemyTypes: Key Elem is name of Enemy, Value Elem is file path to the file containing Enemy details
    using TEnemyTypes = std::unordered_map<Tilc::TExtString, Tilc::TExtString>;

    class TEntityManager
    {
    public:
        TEntityManager(unsigned int MaxEntities);
        ~TEntityManager();
        int Add(const EEntityType Type, const Tilc::TExtString& Name = "");
        TEntityBase* Find(unsigned int Id);
        TEntityBase* Find(const Tilc::TExtString& Name);
        void Remove(unsigned int Id);
        void Update(float DeltaTime);
        void Draw(SDL_Surface* Destination);
        void Purge();
        unsigned int GetMaxEntities() const { return m_MaxEntities; }
        unsigned int AreEnemiesLoaded() const { return m_EnemiesLoaded; }
        unsigned int IsPlayerLoaded() const { return m_PlayerLoaded; }
    private:
        template<class T>
        void RegisterEntity(const EEntityType Type)
        {
            m_EntityFactory[Type] = [this]() -> TEntityBase*
            {
                return new T();
            };
        }
        void ProcessRemovals();
        void LoadEnemyTypes(const Tilc::TExtString& FileName);
        void EntityCollisionCheck();
        TEntityContainer m_Entities;
        TEnemyTypes m_EnemyTypes;
        TEntityFactory m_EntityFactory;
        Tilc::TSharedContext* m_Context = nullptr;
        unsigned int m_IdCounter;
        unsigned int m_MaxEntities;
        TExtVectorContainer<unsigned int> m_EntitiesToRemove;
        bool m_EnemiesLoaded = false;
        bool m_PlayerLoaded = false;
    };
}
