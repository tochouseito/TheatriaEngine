struct ParticlePVA {
    float3 value;
    float3 velocity;
    float3 acceleration;
};
struct Particle {
    ParticlePVA position;
    ParticlePVA rotation;
    ParticlePVA scale;
    float4 color;
    float lifeTime;
    float currentTime;
    uint isAlive;
};
struct PerFrame {
    float time;
    float deltaTime;
};
static const float PI = 3.14159265359;
struct EmitterSphere {
    float3 translate;
    float radius;
    uint count;
    float frequency;
    float frequencyTime;
    uint emit;
};
struct VSOutput {
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
    float4 color : COLOR0;
};