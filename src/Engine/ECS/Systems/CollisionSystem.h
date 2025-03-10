#pragma once

#include <set>
#include <unordered_set>

#include "../Components/Collider3D.h"
#include "ECS/Components/Colliders/SpatialGrid.h"

class ECS;

struct CollisionPair
{
    Collider3D* col1;
    Collider3D* col2;

    CollisionPair(Collider3D* c1, Collider3D* c2)
    {
        if (c1 < c2) { 
            col1 = c1; 
            col2 = c2; 
        } else { 
            col1 = c2; 
            col2 = c1; 
        }
    }

    bool operator==(const CollisionPair& other) const
    {
        return col1 == other.col1 && col2 == other.col2;
    }
};


struct CollisionPairHash {
    std::size_t operator()(const CollisionPair& pair) const {
        return std::hash<Collider3D*>()(pair.col1) ^ std::hash<Collider3D*>()(pair.col2);
    }
};

class CollisionSystem
{
    float mFixedTimestep = 1.0f / 60.0f;
    
public:
    CollisionSystem();
    static CollisionSystem& Get();
    void OnFixedUpdate(ECS* globalEC);
    void HandleOnEvents();
    void DetectCollisions(ECS* globalEC);
    void ProcessCollision(Collider3D* collider1, Collider3D* collider2);
    void UpdateColliders(ECS* globalEC);
    void ResolvePositions();
    void ResolveVelocities();
    bool HasManifold(Collider3D* col1, Collider3D* col2);
    void UpdateManifolds();
    void RemoveEntity(Entity* entity);

private:
    std::vector<CollisionManifold> mManifoldList = {};
    std::unordered_set<CollisionPair, CollisionPairHash> mPreviousCollisions = {};
    std::unordered_set<CollisionPair, CollisionPairHash> mCurrentCollisions = {};
    
    SpatialGrid* mGrid;
    std::vector<Entity*> mNeighboringEntities;
    
    std::set<Entity*> mRemovedEntities;
};
