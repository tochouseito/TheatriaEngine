#include "../header/Particle.hlsli"
#include "../header/Effect.hlsli"
static const uint kMaxParticles = 256;
RWStructuredBuffer<EffectParticle> gParticles : register(u0);
RWStructuredBuffer<int> gFreeListIndex : register(u1);
RWStructuredBuffer<uint> gFreeList : register(u2);
[numthreads(256, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    // 初期化処理
    uint particleIndex = DTid.x;
    
    if (particleIndex < kMaxParticles)
    {
        gParticles[particleIndex] = (EffectParticle) 0;
        gFreeList[particleIndex] = particleIndex;
    }
    // 最初に末尾を入れる
    if (particleIndex == 0)
    {
        gFreeListIndex[0] = kMaxParticles - 1;
    }
}