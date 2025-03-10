#pragma once

template <typename Type, typename ... Args>
Type* ECS::CreateEntity(Args&&... args)
{
    static_assert(std::is_base_of<Entity, Type>::value, "Type must be a subclass of Component");
    Type* entity = new Type(std::forward<Args>(args)...);
    entity->SetIndex(mEntityToAddCount);
    
    EC* ec = new EC(entity);
    mToAddEntities[mEntityToAddCount] = ec;
    mEntityToAddCount++;
    return entity;
}

template <typename T, typename... Args>
T* ECS::AddComponent(Entity* entity, Args&&... args)
{
    static_assert(std::is_base_of<Component, T>::value, "T must be a subclass of Component");
    if (entity->HasComponent<T>()) return GetComponent<T>(entity);
    
    T* component = new T(entity, std::forward<Args>(args)...);
    if (entity->IsCreated())
    {
        mEntities[*entity->GetIndex()]->AttachedComponents.push_back(component);
    } else
    {
        mToAddEntities[*entity->GetIndex()]->AttachedComponents.push_back(component);
    }
    entity->AddBitmask(component->GetBitmask());
    return component;
}

template <typename T>
T* ECS::GetComponent(Entity* entity)
{
    std::vector<Component*> components;
    if (entity->IsCreated())
        components = mEntities[*entity->GetIndex()]->AttachedComponents;
    else
        components = mToAddEntities[*entity->GetIndex()]->AttachedComponents;
    
    for (int i = 0; i < components.size(); i++)
    {
        if (T* foundComp = dynamic_cast<T*>(components[i]))
        {
            return foundComp;
        }
    }
    return nullptr;
}

template <typename T>
T* ECS::GetComponent(int entityIndex)
{
    if (entityIndex > mEntityCount || entityIndex < 0) return nullptr;
     std::vector<Component*>& components = mEntities[entityIndex]->AttachedComponents;
    for (int i = 0; i < components.size(); i++)
    {
        if ( T* foundComp = dynamic_cast<T*>(components[i]))
        {
            return foundComp;
        }
    }
    return nullptr;
}

template <typename T>
bool ECS::HasComponent(const int entityIndex) const
{
    static_assert(std::is_base_of<Component, T>::value, "T must be a subclass of Component");
    if(entityIndex >= mEntityCount) return false;
    Entity* entity = mEntities[entityIndex]->Entity;
    if (entity->IsDestroyed()) return false;
    if (entity->Bitmask & T::BITMASK) return true;
    return false;
}
