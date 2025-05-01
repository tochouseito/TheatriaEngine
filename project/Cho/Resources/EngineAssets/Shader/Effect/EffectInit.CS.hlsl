#include "../header/Effect.hlsli"

// UAV : EffectNode 1024個
RWStructuredBuffer<EffectNode> gEffectNode : register(u0);

// UAV : EffectMeshData 1024個
RWStructuredBuffer<EffectSprite> gEffectMesh : register(u1);
// UAV : MeshDataFreeList 1024個
AppendStructuredBuffer<uint> gEffectMeshFreeList : register(u2);
// UAV : MeshDataFreeListCounter 1個
RWStructuredBuffer<uint> gEffectMeshFreeListCounter : register(u3);

// UAV : EffectParticle 128x1024個
RWStructuredBuffer<EffectParticle> gEffectParticle : register(u4);
// UAV : EffectParticleFreeList 128x1024個
AppendStructuredBuffer<uint> gEffectParticleFreeList : register(u5);
// UAV : ParticleFreeListCounter 1個
RWStructuredBuffer<uint> gEffectParticleFreeListCounter : register(u6);

[numthreads(kMaxParticles, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID, uint3 Gid : SV_GroupID, uint3 GTid : SV_GroupThreadID) {
    // すべてのバッファの初期化
    // 逆順にAppend
    uint i = (128 * kMaxParticles) - DTid.x - 1;
    // 各カウンターを一回だけ初期化
    if (i == 0) {
        gEffectMeshFreeListCounter[0] = 0;
        gEffectParticleFreeListCounter[0] = 0;
    }
    //--Node--//
    if (DTid.x < kMaxNode) {
        gEffectNode[i] = (EffectNode) 0;
    }
    //--MeshData--//
    if (DTid.x < kMaxMeshData) {
        gEffectMesh[i] = (EffectSprite) 0;
        gEffectMeshFreeList.Append(i);
    }
    //--Particle--//
    gEffectParticle[i] = (EffectParticle) 0;
    gEffectParticleFreeList.Append(i);
}