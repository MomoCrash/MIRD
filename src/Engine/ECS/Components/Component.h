#pragma once

class Entity;

class Component
{
public:
    Component(Entity* entity);
    virtual ~Component() = default;
    Entity* GetEntity() const;

    void Activate();
    void Deactivate();
    bool GetState() const;
    virtual int GetBitmask() = 0;
    
protected:
    Entity* mEntity;
    bool mActive;
};
