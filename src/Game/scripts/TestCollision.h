#pragma once
#include "scripts/Script.h"

class TestCollision : public IScript
{
public:
    void OnTriggerEnter(Entity* other) override;
    void OnTriggerStay(Entity* other) override;
    void OnTriggerExit(Entity* other) override;
};
