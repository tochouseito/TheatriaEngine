#include "../header/Effect.hlsli"

// UAV : EffectRoot 128個
RWStructuredBuffer<EffectRoot> gEffectRoot : register(u0);
// UAV : EffectNode 1024個
RWStructuredBuffer<EffectNode> gEffectNode : register(u1);
// UAV : EffectMeshData 1024個
RWStructuredBuffer<EffectSprite> gEffectMesh : register(u2);
// UAV : EffectParticle 128x1024個
RWStructuredBuffer<EffectParticle> gEffectParticle : register(u3);
// UAV : EffectParticleFreeList 128x1024個
AppendStructuredBuffer<uint> gEffectParticleFreeList : register(u4);

[numthreads(kMaxParticles, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID, uint3 Gid : SV_GroupID,uint3 GTid : SV_GroupThreadID) {
    // すべてのバッファの初期化
    uint i = DTid.x;
    //--Root--//
    if (i < kMaxRoot) {
        gEffectRoot[i] = (EffectRoot) 0;
    }
    //--Node--//
    if (i < kMaxNode) {
        gEffectNode[i] = (EffectNode) 0;
    }
    //--MeshData--//
    if (i < kMaxMeshData) {
        gEffectMesh[i] = (EffectSprite) 0;
    }
    //--Particle--//
    gEffectParticle[i] = (EffectParticle) 0;
    gEffectParticleFreeList.Append(i);
}