#include "../header/EffectRoot.hlsli"
#include "../header/EffectNode.hlsli"
#include "../header/EffectSprite.hlsli"
#include "../header/EffectParticle.hlsli"
#include "../header/Random.hlsli"

// SRV : UseRootNumber 128個
StructuredBuffer<uint> gUseRootIDs : register(t0); // 有効なRootのID
// SRV : Root 128個
StructuredBuffer<EffectRoot> gRoots : register(t1);
// SRV : Node 1024個
StructuredBuffer<EffectNode> gNodes : register(t2);
// SRV : Sprite 1024個
StructuredBuffer<EffectSprite> gSprites : register(t3);
// UAV : Particle 128x1024個
RWStructuredBuffer<EffectParticle> gParticles : register(u0);
//-----------------------//
// 他のメッシュ情報を追加予定 //
//-----------------------//

[numthreads(kMaxParticles, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID, uint3 Gid : SV_GroupID, uint3 GTid : SV_GroupThreadID)
{
    uint rootIdx = Gid.x;
    if (rootIdx >= kMaxRoot) return;
    
    // 有効なRootID
    uint rootID = gUseRootIDs[rootIdx];
    //if (rootID == UINT_MAX) return;

    // Root取得
    EffectRoot root = gRoots[rootID];
    // 時間情報を取得
    float elapsed = root.time.elapsedTime;
    float delta = root.time.deltaTime;
    // Rootに紐づくNodeをループ
    for (uint i = 0; i < root.nodeCount; ++i)
    {
        // Node取得
        uint nodeID = FetchEffectNodeID(root, i);
        EffectNode node = gNodes[nodeID];
        // 現在の時間から発生するパーティクルだけ更新
        
        

    }

}