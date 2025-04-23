struct ParticlePVA {
    float3 value;
    float3 velocity;
    float3 acceleration;
};
struct Particle {
    ParticlePVA position;
    ParticlePVA rotation;
    ParticlePVA scale;
    float lifeTime;
    float currentTime;
    float4 color;
    int isAlive;
};
struct PVAVector3 {
    float3 first;
    float3 second;
    uint isMedian;
};

struct PVA {
    PVAVector3 value;
    PVAVector3 velocity;
    PVAVector3 acceleration;
};

struct ParticlePosition {
    uint type;
    float3 value;
    PVA pva;
};

struct ParticleRotation {
    uint type;
    float3 value;
    PVA pva;
};

struct ParticleScale {
    uint type;
    float3 value;
    PVA pva;
};
struct ParticleParameter {
    ParticlePosition position;
    ParticleRotation rotation;
    ParticleScale scale;
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