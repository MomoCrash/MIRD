Texture2D textureMap : register(t0);
SamplerState textureSampler : register(s0);

struct Light
{
    float4 position;
    float4 color;
    float strength;
    float3 padding;
};

cbuffer cbPerObject : register(b0)
{
    float4x4 gWorld;
};

cbuffer cbPass : register(b1)
{
    float4x4 gView;
    float4x4 gViewProj;
    float4 gAmbientLight;
    
    float4 gEyePosW;
    Light gLights[32];
        
    int lightNumber;
    int3 padding;
};

struct VS_VertexOut
{
    float4 PosH : SV_POSITION;
    float3 PosW : POSITION;
    float3 NormalW : NORMAL;
    float2 UV : TEXCOORD;
};


float CalcAttenuation(float d, float falloffStart, float falloffEnd)
{
    // Linear falloff.
    return saturate((falloffEnd - d) / (falloffEnd - falloffStart));
}

float4 PS(VS_VertexOut pin) : SV_Target
{
    
    float4 litColor = gAmbientLight;
    
    for (int i = 0; i < lightNumber; i++)
    {
        float3 direction = pin.PosW - gLights[i].position.rgb;
        float distance = length(direction);
    
        float3 lightDirection = -normalize(direction);
    
        float diffuse = max(dot(lightDirection, normalize(pin.NormalW)), 0.0f);
    
        if (distance > gLights[i].strength+50)
            continue;
    
        // Attenuate light by distance.
        float3 lightStrength = gLights[i].strength * diffuse;
        float att = CalcAttenuation(distance, 1.0f, gLights[i].strength);
        lightStrength *= att;
   
        float4 litMaterial = (diffuse + gAmbientLight) * gLights[i].color;
        litColor += litMaterial;
    }
    litColor /= lightNumber;
    litColor.a = gAmbientLight.a;
    return litColor * textureMap.Sample(textureSampler, pin.UV);
    //return float4(1.0f, 1.0f, 1.0f, 1.0f);
}