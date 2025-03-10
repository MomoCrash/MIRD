#pragma once
#include "ECS.h"
#include "scripts/ScriptManager.h"

class ObjectFactory
{
    
public:
    template <typename T, typename... Args>
    static T* CreateComponent(Entity* entity, Args&&... args);
    template <typename T, typename... Args>
    static T* CreateEntity(Args&&... args);
    template <typename T, typename... Args>
    static T* CreateEntity(Entity* parent, Args&&... args);

    template <typename S>
    static void AttachScript(Entity* parent);
    
};

template <typename CType, typename... Args>
CType* ObjectFactory::CreateComponent(Entity* entity, Args&&... args)
{
    return Engine::GetECS()->AddComponent<CType>(entity, args...);
}

template <typename EType, typename ... Args>
EType* ObjectFactory::CreateEntity(Args&&... args)
{
    static_assert(std::is_base_of<Entity, EType>::value, "EType must be a subclass of Entity");
    return Engine::GetECS()->CreateEntity<EType>(args...);
}

template <typename EType, typename ... Args>
EType* ObjectFactory::CreateEntity(Entity* parent, Args&&... args)
{
    static_assert(std::is_base_of<Entity, EType>::value, "EType must be a subclass of Entity");
    EType* entity = Engine::GetECS()->CreateEntity<EType>(parent, args...);
    //parent->GetTransform()->AddChild(dynamic_cast<Entity*>(entity));
    return entity;
}

template <typename S>
void ObjectFactory::AttachScript(Entity* owner)
{
    Engine::GetScriptManager()->CreateScript<S>(owner);
}
