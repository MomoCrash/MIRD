#pragma once

class Light : public Component
{
public:
    XM_CALLCONV Light(Entity* entity, FXMVECTOR color, float strength);
    
    XMFLOAT3 Color;
    float Strength;
    
    static constexpr int BITMASK = 1 << 6;
    int GetBitmask() override;
};