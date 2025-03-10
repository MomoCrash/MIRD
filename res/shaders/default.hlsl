struct VertexIn
{
 float3 PosL : POSITION;
};

struct VertexOut
{
 float4 PosH : SV_POSITION;
};

VertexOut VS(VertexIn vin)
{
    VertexOut vout = (VertexOut) 0.0f;

    // Transform to homogeneous clip space.
    vout.PosH = float4(vin.PosL, 1.0f);

    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{

    return float4(1.0f, 1.0f, 1.0f, 1.0f);
}


