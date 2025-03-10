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
    
    Light gLight[32];
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