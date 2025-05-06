#include "../header/Effect.hlsli"
#include "../header/Random.hlsli"

// SRV : EffectRoot 128個
StructuredBuffer<EffectRoot> gEffectRoot : register(t0);

// UAV : EffectNode 1024個
RWStructuredBuffer<EffectNode> gEffectNode : register(u0);

// UAV : EffectMeshData 1024個
RWStructuredBuffer<EffectSprite> gEffectMesh : register(u1);
// UAV : MeshDataFreeList 1024個
ConsumeStructuredBuffer<uint> gEffectMeshFreeList : register(u2);

//--元データ--//
// SRV : EffectData 128個
StructuredBuffer<EffectRoot> gEffectData : register(t1);
// SRV : EffectNodeData 1024個
StructuredBuffer<EffectNode> gEffectNodeData : register(t2);
// SRV : EffectMeshData 1024個
StructuredBuffer<EffectSprite> gEffectMeshData : register(t3);

//--発生させるEffectIndex--//
// RootConstant : EffectIndex
cbuffer EffectIndexCB : register(b0) {
    uint gEffectIndex;
};

//--割り当てたRootのインデックス--//
// RootConstant : EffectRootIndex
cbuffer RootIndexCB : register(b1) {
    uint gRootIndex;
};

//--NodeDataIndex--//
// SRV : NodeDataIndex 1024個
StructuredBuffer<uint> gNodeDataIndex : register(t3);
//--MeshDataIndex--//
// SRV : MeshDataIndex 1024個
StructuredBuffer<uint> gMeshDataIndex : register(t4);

/*
EffectEmitはEffectの元データからGPU側にインスタンスを生成するシェーダーです
発生イベントが発火したエフェクトのNodeの数だけDispatchされます
*/
[numthreads(1, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID, uint3 Gid : SV_GroupID, uint3 GTid : SV_GroupThreadID) {
    // Dispatchによって渡されるEffectIndex
    uint dispatchIndex = Gid.x;
    // 元データのEffectDataをRootにコピー
    gEffectRoot[gRootIndex] = gEffectData[gEffectIndex];
    // 割り当てられたRootのNodeに元データNodeをコピー
    uint nodeID = gEffectRoot[gRootIndex].nodeID[dispatchIndex];
    gEffectNode[nodeID] = gEffectNodeData[gNodeDataIndex[dispatchIndex]];
    // スプライトNodeの場合はMeshDataもコピー
    if (gEffectNode[nodeID].draw.meshType == MESH_TYPE_SPRITE) {
        uint meshIndex = gEffectMeshFreeList.Consume();
        gEffectNode[nodeID].draw.meshDataIndex = meshIndex;
        gEffectMesh[meshIndex] = gEffectMeshData[gMeshDataIndex[dispatchIndex]];
    }
}