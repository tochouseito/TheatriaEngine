#include "../header/Effect.hlsli"
#include "../header/Random.hlsli"

// SRV : EffectRoot 128個
StructuredBuffer<EffectRoot> gEffectRoot : register(t0);

// UAV : EffectNode 1024個
RWStructuredBuffer<EffectNode> gEffectNode : register(u0);
// UAV : NodeFreeList 1024個
ConsumeStructuredBuffer<uint> gEffectNodeFreeList : register(u1);
// UAV : NodeFreeListCounter 1個
RWStructuredBuffer<uint> gEffectNodeFreeListCounter : register(u2);

// UAV : EffectMeshData 1024個
RWStructuredBuffer<EffectSprite> gEffectMesh : register(u3);
// UAV : MeshDataFreeList 1024個
ConsumeStructuredBuffer<uint> gEffectMeshFreeList : register(u4);
// UAV : MeshDataFreeListCounter 1個
RWStructuredBuffer<uint> gEffectMeshFreeListCounter : register(u5);

//--元データ--//
// SRV : EffectData 128個
StructuredBuffer<EffectRoot> gEffectData : register(t1);
// SRV : EffectNodeData 1024個
StructuredBuffer<EffectNode> gEffectNodeData : register(t2);
// SRV : EffectMeshData 1024個
StructuredBuffer<EffectSprite> gEffectMeshData : register(t3);

//--発生させるEffectIndex--//
// RootConstant : EffectIndex
ConstantBuffer<uint> gEffectIndex : register(b0);

//--割り当てたRootのインデックス--//
// RootConstant : EffectRootIndex
ConstantBuffer<uint> gRootIndex : register(b1);

//--NodeDataIndex--//
// SRV : NodeDataIndex 1024個
StructuredBuffer<uint> gNodeDataIndex : register(t3);

/*
EffectEmitはEffectの元データからGPU側にインスタンスを生成するシェーダーです
発生イベントが発火したエフェクトのNodeの数だけDispatchされます
*/
[numthreads(1, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID, uint3 Gid : SV_GroupID, uint3 GTid : SV_GroupThreadID) {
    // Dispatchによって渡されるEffectIndex
    uint dispatchIndex = Gid.x;
    // 割り当てられたRootのNodeを設定
    
}