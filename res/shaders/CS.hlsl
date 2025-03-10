// Compute Shader pour mise à jour des particules
struct Particle
{
    float4 position;
    float4 velocity;
};

// Buffers UAV (Unordered Access View)
StructuredBuffer<Particle> ParticlesIn : register(t0); //t0
RWStructuredBuffer<Particle> ParticlesOut : register(u0);

// Constantes
cbuffer SimParams : register(b0)
{
    float4x4 view;
    float3 gravity;
    float deltaTime;
};

[numthreads(256, 1, 1)]
void CSMain(uint3 dispatchThreadID : SV_DispatchThreadID)
{
    uint index = dispatchThreadID.x;
    
    Particle pIn = ParticlesIn[index];
    
    // Mettre la position
    ParticlesOut[index].velocity.xyz = pIn.velocity.xyz;
    ParticlesOut[index].velocity.a = 1.0f;
    ParticlesOut[index].position.xyz = pIn.position.xyz + pIn.velocity.xyz;
    ParticlesOut[index].position.a = 1.0f;

}
