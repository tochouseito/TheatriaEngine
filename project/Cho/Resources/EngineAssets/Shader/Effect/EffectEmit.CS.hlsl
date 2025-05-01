#include "../header/Effect.hlsli"
#include "../header/Random.hlsli"

//--GPU専用バッファ--//
// UAV : EffectRoot 128個
RWStructuredBuffer<EffectRoot> gEffectRoot : register(u0);
// UAV : EffectNode 1024個
RWStructuredBuffer<EffectNode> gEffectNode : register(u1);
// UAV : EffectMeshData 1024個
RWStructuredBuffer<EffectSprite> gEffectMesh : register(u2);
// UAV : EffectParticle 128x1024個
RWStructuredBuffer<EffectParticle> gEffectParticle : register(u3);
// UAV : EffectParticleFreeList 128x1024個
//ConsumeStructuredBuffer<uint> gEffectParticleFreeList : register(u4);

//--元データ--//
// SRV : EffectData 128個
StructuredBuffer<EffectData> gEffectData : register(t0);
// SRV : EffectNodeData 1024個
StructuredBuffer<EffectNodeData> gEffectNodeData : register(t1);
// SRV : EffectMeshData 1024個
StructuredBuffer<EffectSpriteData> gEffectMeshData : register(t2);

//--発生させるEffectIndex--//
// SRV : EffectIndex 128個
StructuredBuffer<uint> gEffectDataIndex : register(t3);

[numthreads(kMaxParticles, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID, uint3 Gid : SV_GroupID, uint3 GTid : SV_GroupThreadID) {
    // Dispatchによって渡されるEffectIndex
    uint dispatchIndex = Gid.x;
    // 乱数生成器
    RandomGenerator random;
}