#pragma once
#include <DirectXMath.h>

class Maths
{
public:
    static constexpr float PI = 3.1415926535f;
    static void Identity4X4(DirectX::XMFLOAT4X4* matrix);
    static DirectX::XMVECTOR SphericalToCartesian(float radius, float theta, float phi);
    static float RandomFloat(float min, float max);
    static int RandomInt(int min, int max);
    
};
