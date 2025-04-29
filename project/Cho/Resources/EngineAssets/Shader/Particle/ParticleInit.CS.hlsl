#include "../header/Particle.hlsli"

static const uint kMaxParticles = 1024;

// UAV: パーティクルリソース
RWStructuredBuffer<Particle> gParticles : register(u0);
// UAV: パーティクルのフリースロットリスト
AppendStructuredBuffer<uint> gFreeList : register(u1);

[numthreads(kMaxParticles, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
    uint particleIndex = DTid.x;
    if (particleIndex < kMaxParticles) {
        gParticles[particleIndex] = (Particle) 0;
        gFreeList.Append(particleIndex);
    }
}
