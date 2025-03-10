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
};


struct VS_VertexIn
{
    float3 PosL : POSITION;
    float3 Norm : NORMAL;
    float3 Tangent : TANGENT;
    float2 UV : TEXCOORD;
};

struct VS_VertexOut
{
    float4 PosH : SV_POSITION;
    float3 PosW : POSITION;
    float3 NormalW : NORMAL;
    float2 UV : TEXCOORD;
};

VS_VertexOut VS(VS_VertexIn vin)
{
    VS_VertexOut vout;
	
    vout.NormalW = mul(vin.Norm, (float3x3) gWorld);
    
    float3 normalView = mul(vout.NormalW, (float3x3) gView);
    
    // Transform to homogeneous clip space.
    float4 posW = mul(float4(vin.PosL, 1.0f), gWorld);
    vout.PosH = mul(posW, gViewProj);
    
    vout.PosW = posW.xyz;
 
    vout.UV = vin.UV;
	
    return vout;
}

float CalcAttenuation(float d, float falloffStart, float falloffEnd)
{
    // Linear falloff.
    return saturate((falloffEnd - d) / (falloffEnd - falloffStart));
}

float4 PS(VS_VertexOut pin) : SV_Target
{
    
    float4 litColor = gAmbientLight;
    float4 base = float4(1.0f, 1.0f, 1.0f, 1.0f);
    
    for (int i = 0; i < 1; i++)
    {
        float3 direction = pin.PosW - gLights[i].position.rgb;
        float distance = length(direction);
    
        float3 lightDirection = -normalize(direction);
    
        float diffuse = max(dot(lightDirection, normalize(pin.NormalW)), 0.0f);
    
        if (distance > 100.0f)
            continue;
    
        // Attenuate light by distance.
        float3 lightStrength = gLights[i].strength * diffuse;
        float att = CalcAttenuation(distance, 1.0f, 120.0f);
        lightStrength *= att;
   
        float4 litMaterial = (diffuse + gAmbientLight) * gLights[i].color * att;
        litColor += litMaterial;
    }
    litColor.a = gAmbientLight.a;
    return litColor * textureMap.Sample(textureSampler, pin.UV);
    //return float4(1.0f, 1.0f, 1.0f, 1.0f);
}