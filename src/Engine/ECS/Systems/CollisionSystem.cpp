#include "pch.h"
#include "CollisionSystem.h"

#include "ECS/ECS.h"
#include "ECS/Components/PhysicsMaterial.h"
#include "ECS/Components/RigidBody3D.h"
#include "ECS/Components/Colliders/UtilsCollider.h"
#include "scripts/ScriptManager.h"
#include "Utils/Profiler.h"

int MAX_ITERATIONS = 10;
float PENETRATION_SLACK = 0.25f;

CollisionSystem::CollisionSystem()  
{
    mGrid = new SpatialGrid(25.0f, -4,0,-4,4,4,4);
}

CollisionSystem& CollisionSystem::Get()
{
    static CollisionSystem instance;    
    return instance;
}

bool CollisionSystem::HasManifold(Collider3D* collider1, Collider3D* collider2)
{
    CollisionPair pair{collider1, collider2};

    if (mCurrentCollisions.find(pair) != mCurrentCollisions.end()) return true;

    for (const auto& manifold : mManifoldList)
    {
        if ((manifold.collider1 == pair.col1 && manifold.collider2 == pair.col2) ||
            (manifold.collider1 == pair.col2 && manifold.collider2 == pair.col1))
        {
            return true;
        }
    }

    return false;
}


void CollisionSystem::DetectCollisions(ECS* globalEC)
{
    Profiler collisionProfiler;
    collisionProfiler.NewTask("Detect collisions");

    auto allCells = mGrid->GetAllCells();
    
    for (auto& [coords, entitiesInCell] : allCells)
    {
        
        mGrid->GetNeighboringEntities(coords.x, coords.y, coords.z, mNeighboringEntities);
        
        for (Entity* entity1 : entitiesInCell)
        {
            if (mRemovedEntities.contains(entity1)) continue;
            if(!entity1 || entity1->IsDestroyed()) continue;
            
            Collider3D* collider1 = globalEC->GetComponent<Collider3D>(*entity1->GetIndex());
            if (!collider1 || !collider1->GetState()) continue;
            
            for (Entity* entity2 : mNeighboringEntities)
            {
                if (mRemovedEntities.contains(entity2)) continue;
                if (entity2->GetTag() == entity1->GetTag() || entity2->IsDestroyed()) continue;
                
                Collider3D* collider2 = globalEC->GetComponent<Collider3D>(*entity2->GetIndex());
                if (!collider2 || !collider2->GetState() || (collider1->IsStatic() && collider2->IsStatic())) continue;

                if (HasManifold(collider1, collider2)) continue;

                ProcessCollision(collider1, collider2);
            }
        }
    }

    collisionProfiler.EndTask();
}

void CollisionSystem::ProcessCollision(Collider3D* collider1, Collider3D* collider2)
{
    CollisionManifold manifold = collider1->Intersects(collider2);
    if (manifold.hasCollision && manifold.penetrationDepth > PENETRATION_SLACK)
    {
        manifold.collider1 = collider1;
        manifold.collider2 = collider2;
        mManifoldList.push_back(manifold);
    }
}

//TODO More Opti
void CollisionSystem::UpdateColliders(ECS* globalEC)
{
    for (int i = 0; i < globalEC->mEntityCount; i++)
    {
        if (!globalEC->HasComponent<Collider3D>(i)) continue;

        Collider3D* collider = globalEC->GetComponent<Collider3D>(i);
        Entity* entity = collider->GetEntity();
        collider->SetCenter(XMLoadFloat3(&entity->GetTransform()->position));

        mGrid->UpdateEntity(entity);
    }
}

void CollisionSystem::ResolvePositions()
{
    for (auto& manifold : mManifoldList)
    {
        Collider3D* collider1 = manifold.collider1;
        Collider3D* collider2 = manifold.collider2;

        if(collider1->IsTrigger() || collider2->IsTrigger())
            continue;
        
        Entity* entity1 = collider1->GetEntity();
        Entity* entity2 = collider2->GetEntity();
        
        RigidBody3D* rb1 = entity1->HasComponent<RigidBody3D>() ? entity1->GetComponent<RigidBody3D>() : nullptr;
        RigidBody3D* rb2 = entity2->HasComponent<RigidBody3D>() ? entity2->GetComponent<RigidBody3D>() : nullptr;
        
        float totalInvMass = 0.0f;
        if (rb1 && !collider1->IsStatic() && !collider1->IsTrigger()) totalInvMass += rb1->GetInvMass();
        if (rb2 && !collider2->IsStatic() && !collider2->IsTrigger()) totalInvMass += rb2->GetInvMass();
        
        if (totalInvMass <= 0.0f) continue;
        
        float correctionAmount = manifold.penetrationDepth / totalInvMass;
        
        XMVECTOR collisionNormal = XMLoadFloat3(&manifold.normal);
        
        if (rb1 && !collider1->IsStatic())
        {
            float ratio1 = rb1->GetInvMass() / totalInvMass;
            XMVECTOR pos = XMLoadFloat3(&entity1->GetTransform()->position);
            XMVECTOR correction = XMVectorScale(collisionNormal, correctionAmount * ratio1 * mFixedTimestep);
            pos = XMVectorAdd(pos, correction);
            XMStoreFloat3(&entity1->GetTransform()->position, pos);
            collider1->SetCenter(pos);
        }
        
        if (rb2 && !collider2->IsStatic())
        {
            float ratio2 = rb2->GetInvMass() / totalInvMass;
            XMVECTOR pos = XMLoadFloat3(&entity2->GetTransform()->position);
            XMVECTOR correction = XMVectorScale(collisionNormal, -correctionAmount * ratio2 * mFixedTimestep);
            pos = XMVectorAdd(pos, correction);
            XMStoreFloat3(&entity2->GetTransform()->position, pos);
            collider2->SetCenter(pos);
        }
    }
}

void CollisionSystem::ResolveVelocities()
{
    for (auto& manifold : mManifoldList)
    {
        Collider3D* collider1 = manifold.collider1;
        Collider3D* collider2 = manifold.collider2;

        if(collider1->IsTrigger() || collider2->IsTrigger())
            continue;
        
        Entity* entity1 = collider1->GetEntity();
        Entity* entity2 = collider2->GetEntity();
        
        RigidBody3D* rb1 = entity1->HasComponent<RigidBody3D>() ? entity1->GetComponent<RigidBody3D>() : nullptr;
        RigidBody3D* rb2 = entity2->HasComponent<RigidBody3D>() ? entity2->GetComponent<RigidBody3D>() : nullptr;
        
        if (!rb1 && !rb2) continue;
        
        XMVECTOR collisionNormal = XMLoadFloat3(&manifold.normal);
        
        XMVECTOR velocity1 = rb1 ? XMLoadFloat3(&rb1->mVelocity) : XMVectorZero();
        XMVECTOR velocity2 = rb2 ? XMLoadFloat3(&rb2->mVelocity) : XMVectorZero();
        
        XMVECTOR relativeVelocity = XMVectorSubtract(velocity2, velocity1);
        
        float velAlongNormal = XMVectorGetX(XMVector3Dot(relativeVelocity, collisionNormal));
        
        PhysicsMaterial* pm1 = entity1->GetComponent<PhysicsMaterial>();
        PhysicsMaterial* pm2 = entity2->GetComponent<PhysicsMaterial>();
                                
        float elasticity1 = pm1->mElasticity;
        float elasticity2 = pm2->mElasticity;
        
        float e = 0.5f * (elasticity1 + elasticity2);
        
        float totalInvMass = 0.0f;
        if (rb1 && !collider1->IsStatic() && !collider1->IsTrigger()) totalInvMass += rb1->GetInvMass();
        if (rb2 && !collider2->IsStatic() && !collider2->IsTrigger()) totalInvMass += rb2->GetInvMass();
        
        if (totalInvMass <= 0.0f) continue;
        
        float j = -(1.0f + e) * velAlongNormal;
        j /= totalInvMass;
        
        XMVECTOR impulse = XMVectorScale(collisionNormal, j);
        
        if (rb1 && !collider1->IsStatic() && !collider1->IsTrigger())
        {
            rb1->ApplyImpulse(-impulse);
        }
        
        if (rb2 && !collider2->IsStatic() && !collider2->IsTrigger())
        {
            rb2->ApplyImpulse(impulse);
        }
    }
}

void CollisionSystem::UpdateManifolds()
{
    std::vector<CollisionManifold> updatedManifolds;

    for (const auto& manifold : mManifoldList)
    {
        Collider3D* collider1 = manifold.collider1;
        Collider3D* collider2 = manifold.collider2;
        
        CollisionManifold updatedManifold = collider1->Intersects(collider2);
        
        if (updatedManifold.hasCollision)
        {
            updatedManifold.collider1 = collider1;
            updatedManifold.collider2 = collider2;
            if (updatedManifold.penetrationDepth > PENETRATION_SLACK)
                updatedManifolds.push_back(updatedManifold);
        }
    }

    mManifoldList = updatedManifolds;
}


void CollisionSystem::OnFixedUpdate(ECS* globalEC)
{
    mManifoldList.clear();
    mCurrentCollisions.clear();
    
    UpdateColliders(globalEC);
    
    DetectCollisions(globalEC);

    for (auto& manifold : mManifoldList)
    {
        CollisionPair pair(manifold.collider1, manifold.collider2);
        mCurrentCollisions.insert(pair);
    }

    for (int iteration = 0; iteration < MAX_ITERATIONS; iteration++)
    {
        if(mManifoldList.empty())
            break;

        
        bool stable = true;
        for (const auto& manifold : mManifoldList)
        {
            if (manifold.penetrationDepth > PENETRATION_SLACK)
            {
                stable = false;
                break;
            }

            if (iteration < MAX_ITERATIONS - 1)
            {
                UpdateManifolds();
            }
        }
        
        if (stable) break;
                
        ResolvePositions();
        
        ResolveVelocities();
        
    }
    
    HandleOnEvents();

    mPreviousCollisions = mCurrentCollisions;

    for(Entity* entityRemoved : mRemovedEntities)
    {
        for (auto& [coords, entitiesInCell] : mGrid->GetAllCells())
        {
            int toRemove = 0;
            for (auto entity : entitiesInCell)
            {
                if (entity == entityRemoved)
                {
                    entitiesInCell.erase(entitiesInCell.begin()+toRemove);
                    break;
                }
                toRemove++;
            }
        }
    }
    mRemovedEntities.clear();

}

void CollisionSystem::HandleOnEvents()
{
    for (const auto& pair : mCurrentCollisions)
    {
        bool wasCollidingLastFrame = (mPreviousCollisions.find(pair) != mPreviousCollisions.end());
        if (!wasCollidingLastFrame)
        {
            if (pair.col1->IsTrigger())
                Engine::GetScriptManager()->OnTriggerEnter(pair.col1->GetEntity(), pair.col2->GetEntity());
            else
                Engine::GetScriptManager()->OnCollisionEnter(pair.col1->GetEntity(), pair.col2->GetEntity());

            if (pair.col2->IsTrigger())
                Engine::GetScriptManager()->OnTriggerEnter(pair.col2->GetEntity(), pair.col1->GetEntity());
            else
                Engine::GetScriptManager()->OnCollisionEnter(pair.col2->GetEntity(), pair.col1->GetEntity());
        }
        else
        {
            if (pair.col1->IsTrigger())
                Engine::GetScriptManager()->OnTriggerStay(pair.col1->GetEntity(),pair.col2->GetEntity());
            else
                Engine::GetScriptManager()->OnCollisionStay(pair.col1->GetEntity(),pair.col2->GetEntity());

            if (pair.col2->IsTrigger())
                Engine::GetScriptManager()->OnTriggerStay(pair.col2->GetEntity(), pair.col1->GetEntity());
            else
                Engine::GetScriptManager()->OnCollisionStay(pair.col2->GetEntity(),pair.col1->GetEntity());
        }
    }

    for (const auto& pair : mPreviousCollisions)
    {
        if (mCurrentCollisions.find(pair) == mCurrentCollisions.end())
        {
            if (pair.col1->IsTrigger())
                Engine::GetScriptManager()->OnTriggerExit(pair.col1->GetEntity(), pair.col2->GetEntity());
            else
                Engine::GetScriptManager()->OnCollisionExit(pair.col1->GetEntity(), pair.col2->GetEntity());

            if (pair.col2->IsTrigger())
                Engine::GetScriptManager()->OnTriggerExit(pair.col2->GetEntity(), pair.col1->GetEntity());
            else
                Engine::GetScriptManager()->OnCollisionExit(pair.col2->GetEntity(), pair.col1->GetEntity());
        }
    }
}

void CollisionSystem::RemoveEntity(Entity* entity)
{
    mRemovedEntities.emplace(entity);
}
