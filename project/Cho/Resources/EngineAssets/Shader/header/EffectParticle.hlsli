struct EffectParticlePVA
{
    float3 value;
    float3 velocity;
    float3 acceleration;
};
struct EffectParticle
{
    EffectParticlePVA position;
    EffectParticlePVA rotation;
    EffectParticlePVA scale;
    float4 color;
    float lifeTime;
    float spawnTime;
    uint isAlive;
    uint nodeID;
};
static const uint kMaxParticles = 1024;