struct VertexShaderOutput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
    float4 color : COLOR0;
};

static const float PI = 3.14159265359;

struct Particle
{
    float3 translate;
    float3 rotate;
    float3 scale;
    float lifeTime;
    float3 velocity;
    float currentTime;
    float4 color;
    int isAlive;
};

struct ParticlePVA
{
    float3 value;
    float3 velocity;
    float3 acceleration;
};

struct EffectParticle
{
    ParticlePVA position;
    ParticlePVA rotation;
    ParticlePVA scale;
    float lifeTime;
    float currentTime;
    float4 color;
    int isAlive;
};
struct PerFrame
{
    float time;
    float deltaTime;
};
struct EmitterSphere
{
    float3 translate;
    float radius;
    uint count;
    float frequency;
    float frequencyTime;
    uint emit;
};
