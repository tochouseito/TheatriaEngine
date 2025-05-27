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
    uint isFadeOut;
    uint isBillboard;
    uint isAlive;
    uint materialID;
};
struct PerFrame {
    float time;
    float deltaTime;
};
static const float PI = 3.14159265359;
struct RandValue
{
    float median;
    float amplitude;
};

struct RandVector3
{
    RandValue x;
    RandValue y;
    RandValue z;
};
struct PVASRT
{
    RandVector3 value;
    RandVector3 velocity;
    RandVector3 acceleration;
};
struct EmitterSphere {
    RandValue lifeTime;
    PVASRT position;
    PVASRT rotation;
    PVASRT scale;
    float frequency;
    float frequencyTime;
    uint emit;
    uint emitCount;
    uint isFadeOut;
    uint isBillboard;
    uint materialID;
};
struct VSOutput {
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
    float4 color : COLOR0;
    uint materialID : MATERIALID0;
};