#pragma once
#include <map>

struct IScript;

class ScriptManager
{
public:
    ScriptManager();
    
    template <typename SType>
    void CreateScript(Entity* entity);
    void RemoveEntity(int* index);

    void OnCollisionEnter(Entity* collider, Entity* other);
    void OnTriggerEnter(Entity* collider, Entity* other);

    void OnCollisionStay(Entity* collider, Entity* other);
    void OnTriggerStay(Entity* collider, Entity* other);

    void OnCollisionExit(Entity* collider, Entity* other);
    void OnTriggerExit(Entity* collider, Entity* other);

    void OnPreRender();
    void OnFixedUpdate();
    void OnUpdate();

private:
    std::map<int*, std::vector<IScript*>> scriptedEntity;
    std::map<int*, std::vector<IScript*>> scriptedEntityToAdd;
    int mEntityToRemoveCount;
    int** toRemoveId = new int*[1024];
    
};

template <typename SType>
void ScriptManager::CreateScript(Entity* entity)
{
    SType* script = new SType();
    script->SetOwner(entity);
    scriptedEntityToAdd[entity->GetIndex()].push_back(script);
}