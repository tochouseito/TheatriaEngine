#include "../header/Effect.hlsli"

// UAV : EffectRoot 128個
RWStructuredBuffer<EffectRoot> gEffectRoot : register(u0);
// UAV : RootFreeList 128個
AppendStructuredBuffer<uint> gEffectRootFreeList : register(u1);
// UAV : RootFreeListCounter 1個
RWStructuredBuffer<uint> gEffectRootFreeListCounter : register(u2);

// UAV : EffectNode 1024個
RWStructuredBuffer<EffectNode> gEffectNode : register(u3);
// UAV : NodeFreeList 1024個
AppendStructuredBuffer<uint> gEffectNodeFreeList : register(u4);
// UAV : NodeFreeListCounter 1個
RWStructuredBuffer<uint> gEffectNodeFreeListCounter : register(u5);

// UAV : EffectMeshData 1024個
RWStructuredBuffer<EffectSprite> gEffectMesh : register(u6);
// UAV : MeshDataFreeList 1024個
AppendStructuredBuffer<uint> gEffectMeshFreeList : register(u7);
// UAV : MeshDataFreeListCounter 1個
RWStructuredBuffer<uint> gEffectMeshFreeListCounter : register(u8);

// UAV : EffectParticle 128x1024個
RWStructuredBuffer<EffectParticle> gEffectParticle : register(u9);
// UAV : EffectParticleFreeList 128x1024個
AppendStructuredBuffer<uint> gEffectParticleFreeList : register(u10);
// UAV : ParticleFreeListCounter 1個
RWStructuredBuffer<uint> gEffectParticleFreeListCounter : register(u11);

[numthreads(kMaxParticles, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID, uint3 Gid : SV_GroupID, uint3 GTid : SV_GroupThreadID) {
    // すべてのバッファの初期化
    // 逆順にAppend
    uint i = (128 * kMaxParticles) - DTid.x - 1;
    // 各カウンターを一回だけ初期化
    if (i == 0) {
        gEffectRootFreeListCounter[0] = 0;
        gEffectNodeFreeListCounter[0] = 0;
        gEffectMeshFreeListCounter[0] = 0;
        gEffectParticleFreeListCounter[0] = 0;
    }
    //--Root--//
    if (DTid.x < kMaxRoot) {
        gEffectRoot[i] = (EffectRoot) 0;
        gEffectRootFreeList.Append(i);
    }
    //--Node--//
    if (DTid.x < kMaxNode) {
        gEffectNode[i] = (EffectNode) 0;
        gEffectNodeFreeList.Append(i);
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