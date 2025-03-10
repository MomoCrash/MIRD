#include "pch.h"
#include "ScriptManager.h"

#include "Script.h"
#include "ECS/Entity.h"

ScriptManager::ScriptManager(): mEntityToRemoveCount(0) {}

void ScriptManager::RemoveEntity(int* index)
{
    toRemoveId[mEntityToRemoveCount++] = index;
}

void ScriptManager::OnCollisionEnter(Entity* collider, Entity* other)
{
    for (IScript* script : scriptedEntity[collider->GetIndex()])
    {
        script->OnCollisionEnter(other);
    }
}

void ScriptManager::OnTriggerEnter(Entity* collider, Entity* other)
{
    for (IScript* script : scriptedEntity[collider->GetIndex()])
    {
        script->OnTriggerEnter(other);
    }
}

void ScriptManager::OnCollisionStay(Entity* collider, Entity* other)
{
    for (IScript* script : scriptedEntity[collider->GetIndex()])
    {
        script->OnCollisionStay(other);
    }
}

void ScriptManager::OnTriggerStay(Entity* collider, Entity* other)
{
    for (IScript* script : scriptedEntity[collider->GetIndex()])
    {
        script->OnTriggerStay(other);
    }
}

void ScriptManager::OnCollisionExit(Entity* collider, Entity* other)
{
    for (IScript* script : scriptedEntity[collider->GetIndex()])
    {
        script->OnCollisionExit(other);
    }
}

void ScriptManager::OnTriggerExit(Entity* collider, Entity* other)
{
    for (IScript* script : scriptedEntity[collider->GetIndex()])
    {
        script->OnTriggerExit(other);
    }
}

void ScriptManager::OnPreRender()
{
    Engine::GetRenderWindow()->OpenCommandList();
    for (auto& idScripts : scriptedEntity)
    {
        for (IScript* script : idScripts.second)
        {
            if (script->owner->IsDestroyed()) continue;
            script->OnPreRender();
        }
    }
    Engine::GetRenderWindow()->CloseCommandList();
}

void ScriptManager::OnFixedUpdate()
{
    for (auto& idScripts : scriptedEntity)
    {
        for (IScript* script : idScripts.second)
        {
            if (script->owner->IsDestroyed()) continue;
            script->OnFixedUpdate();
        }
    }
}

void ScriptManager::OnUpdate()
{

    Engine::GetRenderWindow()->OpenCommandList();
    for (auto& idScriptToAdd : scriptedEntityToAdd)
    {
        for (IScript* script : idScriptToAdd.second)
        {
            script->OnStart();
            scriptedEntity[idScriptToAdd.first].push_back(script);
        }
    }
    Engine::GetRenderWindow()->CloseCommandList();
    scriptedEntityToAdd.clear();

    for (int i = 0; i < mEntityToRemoveCount; i++)
    {
        int* indexToRemove = toRemoveId[i];
        for (IScript* script : scriptedEntity[indexToRemove])
        {
            delete script;
        }
        scriptedEntity.erase(indexToRemove);
    }
    mEntityToRemoveCount = 0;
    
    for (auto& idScripts : scriptedEntity)
    {
        for (IScript* script : idScripts.second)
        {
            script->OnUpdate();
        }
    }
}
