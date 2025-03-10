cbuffer SimParams : register(b0)
{
    float4x4 viewProj;
    float3 gravity;
    float deltaTime;
};

cbuffer PerParticleCb : register(b1)
{
    float4x4 gWorld;
};

struct VS_VertexIn
{
    float4 PosL : POSITION;
    float4 Velocity : VELOCITY;
};

struct VS_VertexOut {
    float4 PosH : SV_POSITION;
};

VS_VertexOut VS(VS_VertexIn vin)
{
    VS_VertexOut vout;
    float4 posW = mul(vin.PosL, gWorld);
    vout.PosH = mul(posW, viewProj);
    return vout;
}

float4 PS(VS_VertexOut pin) : SV_Target
{
    return float4(0.0f, 0.50f, 0.50f, 1.0f);
}