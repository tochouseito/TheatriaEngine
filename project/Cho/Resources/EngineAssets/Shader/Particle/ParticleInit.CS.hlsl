#include "../header/Particle.hlsli"
static const uint kMaxParticles = 1024;

// UAV: パーティクルリソース
RWStructuredBuffer<Particle> gParticles : register(u0);
// UAV: パーティクルのフリースロットリストインデックス
RWStructuredBuffer<int> gFreeListIndex : register(u1);
// UAV: パーティクルのフリースロットリスト
RWStructuredBuffer<uint> gFreeList : register(u2);

[numthreads(kMaxParticles, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
    // 初期化処理
    uint particleIndex = DTid.x;
    
    if (particleIndex < kMaxParticles) {
        gParticles[particleIndex] = (Particle) 0;
        gFreeList[particleIndex] = particleIndex;
    }
    // 最初に末尾を入れる
    if (particleIndex == 0) {
        gFreeListIndex[0] = kMaxParticles - 1;
    }
}