#pragma once
#include <cstdint>

struct TRANSFORM;

class Entity
{
public:
    Entity();
    Entity(Entity* parent);
    ~Entity();

    enum class Tag : uint8_t
    {
        NONE,
        PLAYER,
        GROUND,
        OBSTACLE,
        ENNEMY,
        PROJECTILES,
        ENEMY_PROJECTILES,
        BUMPER,

        TAG_COUNT
    };

    template<typename T>
    T* GetComponent();
    template<typename T>
    bool HasComponent() const;

    void AddBitmask(int mask);
    void RemoveBitmask(int mask);
    void SetIndex(int index);
    int GetId();
    int* GetIndex();

    void Create(int index);
    void Destroy();

    bool IsCreated() const;
    bool IsDestroyed() const;
    
    void SetTag(Tag nTag) { mTag = nTag; }
    bool IsTag(Tag oTag) const { return mTag == oTag; }
    [[nodiscard]] Tag GetTag() const { return mTag; }
    
    TRANSFORM* GetTransform() const {return mTransform; }
    int Bitmask;

protected:
    bool mCreated;
    bool mDestoyed;
    int mId;
    int mIndex;
    Tag mTag;
    
private:
    TRANSFORM* mTransform;
    static int sNextId;
};

#include "Entity.inl"