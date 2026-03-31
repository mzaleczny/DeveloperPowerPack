#pragma once
#include "Tilc/DllGlobals.h"
#include "Tilc/Utils/Vector.h"
#include "Tilc/Utils/VectorContainer.h"
#include "SDL3/SDL.h"
#include <string>

namespace Tilc
{
    class TGame;
}

namespace Tilc::Game2D
{
    class TTileInfo;
    class TMapLayer;
    class TWorld2D;
    class TEntityManager;

    enum class EEntityType
    {
        Base,
        Enemy,
        Player
    };

    enum class EEntityState
    {
        Idle,
        Walk,
        Jump,
        Attack,
        Hurt,
        Die
    };

    struct TCollisionElement
    {
        TCollisionElement(float Area, TTileInfo* TileInfo, const SDL_FRect& Bounds)
            : m_Area(Area), m_Tile(TileInfo), m_TileBounds(Bounds)
        {}
        float m_Area;
        TTileInfo* m_Tile;
        SDL_FRect m_TileBounds;
    };
    using TCollisions = TExtVectorContainer<TCollisionElement>;
    
    class DECLSPEC TEntityBase
    {
        friend class TEntityManager;
    public:
        // ReferenceLayerIndex is an index of layer from which there will be obtained information on Map Size and Tile Size for calculating Entity Movement
        // and collisons checking
        TEntityBase();
        virtual ~TEntityBase();
        inline void SetPosition(float x, float y)
        {
            m_Position.x = x;
            m_Position.y = y;
            UpdateBoundingBox();
        }
        inline void SetSize(float x, float y)
        {
            m_Size.x = x;
            m_Size.y = y;
            UpdateBoundingBox();
        }
        virtual void SetState(EEntityState State);
        void Move(float dx, float dy);
        void AddVelocity(float dx, float dy);
        inline void Accelerate(float dx, float dy)
        {
            m_Acceleration.x += dx;
            m_Acceleration.y += dy;
        }
        inline void SetAcceleration(float ax, float ay)
        {
            m_Acceleration.x = ax;
            m_Acceleration.y = ay;
        }
        void ApplyFriction(float fx, float fy);
        virtual void Update(float DeltaTime);
        virtual void Draw(SDL_Surface* Destination) = 0;
        inline EEntityType GetType() const
        {
            return m_Type;
        }
        const Tilc::TVector2D<float>& GetPosition() const
        {
            return m_Position;
        }
    protected:
        void UpdateBoundingBox()
        {
            m_BoundingBox = { m_Position.x, m_Position.y, m_Size.x, m_Size.y };
        }
        void CheckCollisions();
        void ResolveCollisions();
        // Method for what THIS entity does TO the Collider Entity.
        virtual void OnEntityCollision(TEntityBase* Collider, bool Attack) = 0;
        std::string m_Name;
        EEntityType m_Type;
        unsigned int m_Id; // Entity id in the entity manager.
        Tilc::TVector2D<float> m_Size; // Current size.
        Tilc::TVector2D<float> m_Position; // Current position.
        Tilc::TVector2D<float> m_PositionOld; // Position before entity moved.
        Tilc::TVector2D<float> m_Velocity; // Current velocity: how fast an Entity is moving - the number of pixels that an entity moves across per second
        Tilc::TVector2D<float> m_MaxVelocity; // Maximum velocity.
        Tilc::TVector2D<float> m_Speed; // Speed
        Tilc::TVector2D<float> m_Acceleration; // Current acceleration: how fast the Entity's velocity increases - number of pixels per second that get added to the Entity's velocity.
        Tilc::TVector2D<float> m_Friction; // Default friction value: Friction defines how much of the entity's velocity is lost to the environment.
        TTileInfo* m_GroundTile; // Tile underneath entity.
        Tilc::TVector2D<float> m_ColliderSize; // Size of the collision box.
        SDL_FRect m_BoundingBox; // The bounding box for collisions - it holds two pairs of different values: the position and the size.
        EEntityState m_State; // Current entity state.
        // Flags for remembering axis collisions.
        bool m_CollidingOnX;
        bool m_CollidingOnY;
        TCollisions m_Collisions;
        TMapLayer* ReferenceLayer = nullptr;

    public:
        TGame* GameObject = nullptr;
        TWorld2D* World = nullptr;
        TEntityManager* m_EntityManager = nullptr;
    };
}
