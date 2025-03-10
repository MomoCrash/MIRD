#pragma once
#include "scripts/Script.h"

class Bumper : public IScript
{
public:
    void OnTriggerEnter(Entity* other) override;
};
