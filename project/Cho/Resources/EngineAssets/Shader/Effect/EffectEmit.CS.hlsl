#include "../header/EffectRoot.hlsli"
#include "../header/EffectNode.hlsli"
#include "../header/EffectSprite.hlsli"
#include "../header/EffectParticle.hlsli"
#include "../header/Random.hlsli"

// SRV : UseRootNumber 128個
StructuredBuffer<uint> gUseRootIDs : register(t0);// 有効なRootのID
// SRV : Root 128個
StructuredBuffer<EffectRoot> gRoots : register(t1);
// SRV : Node 1024個
StructuredBuffer<EffectNode> gNodes : register(t2);
// SRV : Sprite 1024個
StructuredBuffer<EffectSprite> gSprites : register(t3);
// UAV : Particle 128x1024個
RWStructuredBuffer<EffectParticle> gParticles : register(u0);
// UAV : ParticleFreeList 128x1024個
ConsumeStructuredBuffer<uint> gParticleFreeList : register(u1);
// UAV : ParticleFreeListCounter 1個
RWStructuredBuffer<uint> gParticleCounter : register(u2);
//-----------------------//
// 他のメッシュ情報を追加予定 //
//-----------------------//

// 現フレームで発生すべきパーティクル数
uint CalcSpawnCount(
    in EffectCommon common,
    float elapsed,
    in RandomGenerator generator)
{
    // 発生開始時間を設定
    float emitStartTime = GenerateRandomInRange(generator.Generate1d(), common.emitStartTime.median, common.emitStartTime.amplitude);
    
    // 発生開始前なら 0
    if (elapsed < emitStartTime)
    {
        return 0;
    }

    // 経過時間からサイクル数を計算
    float t = elapsed - emitStartTime;
    uint totalAllowed;
    // 発生間隔を設定
    float emitInterval = GenerateRandomInRange(generator.Generate1d(), common.emitInterval.median, common.emitInterval.amplitude);
    if (emitInterval <= 0.0f)
    {
        // 間隔ゼロ=瞬時に一度だけ emitCount 発生
        totalAllowed = common.emitCount;
    }
    else
    {
        // 第何サイクル目か（0-based）を floor で取得し +1
        float cycle = t / emitInterval;
        uint numCycles = (uint) floor(cycle) + 1;
        totalAllowed = numCycles * common.emitCount;
    }

    // 上限をかける
    totalAllowed = min(totalAllowed, common.emitCountMax);

    return totalAllowed;
}

[numthreads(1, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID, uint3 Gid : SV_GroupID)
{
    uint rootIdx = DTid.x;
    if (rootIdx >= kMaxRoot)
        return;
    
    // 有効なRootID
    uint rootID = gUseRootIDs[rootIdx];
    //if (rootID == UINT_MAX) return;

    // Root取得
    EffectRoot root = gRoots[rootID];
    // 時間情報を取得
    float elapsed = root.time.elapsedTime;
    float delta = root.time.deltaTime;
    // シード生成
    RandomGenerator generator;
    generator.seed = (DTid + elapsed)*elapsed;
    // Rootに紐づくNodeをループ
    for (uint i = 0; i < root.nodeCount; ++i)
    {
        // Node取得
        uint nodeID = FetchEffectNodeID(root, i);
        EffectNode node = gNodes[nodeID];
        // 生成するパーティクル数を計算
        uint spawnCount = CalcSpawnCount(node.common, elapsed, generator);
        // 生成するパーティクル数が 0 ならスキップ
        if (spawnCount == 0)
        {
            continue;
        }
        // 生成
        for (uint pi = 0; pi < spawnCount; ++pi)
        {
            // パーティクルインデックスを取得
            if (gParticleCounter[0] == 0)
            {
                break; // 空き無し
            }
            uint particleIdx = gParticleFreeList.Consume();
            // パーティクル初期化
            // Rotation
            switch (gEffectNode[nodeIndex].rotation.type)
            {
                case SRT_TYPE_STANDARD:
                    gEffectParticle[particleIndex].rotation.value = gEffectNode[nodeIndex].rotation.value;
                    break;
                case SRT_TYPE_PVA:
                    gEffectParticle[particleIndex].rotation.value = float3(
                            GenerateRandomInRange(generator.Generate1d(), gEffectNode[nodeIndex].rotation.pva.value.x.median, gEffectNode[nodeIndex].rotation.pva.value.x.amplitude),
                            GenerateRandomInRange(generator.Generate1d(), gEffectNode[nodeIndex].rotation.pva.value.y.median, gEffectNode[nodeIndex].rotation.pva.value.y.amplitude),
                            GenerateRandomInRange(generator.Generate1d(), gEffectNode[nodeIndex].rotation.pva.value.z.median, gEffectNode[nodeIndex].rotation.pva.value.z.amplitude)
                        );
                    gEffectParticle[particleIndex].rotation.velocity = float3(
                            GenerateRandomInRange(generator.Generate1d(), gEffectNode[nodeIndex].rotation.pva.velocity.x.median, gEffectNode[nodeIndex].rotation.pva.velocity.x.amplitude),
                            GenerateRandomInRange(generator.Generate1d(), gEffectNode[nodeIndex].rotation.pva.velocity.y.median, gEffectNode[nodeIndex].rotation.pva.velocity.y.amplitude),
                            GenerateRandomInRange(generator.Generate1d(), gEffectNode[nodeIndex].rotation.pva.velocity.z.median, gEffectNode[nodeIndex].rotation.pva.velocity.z.amplitude)
                        );
                    gEffectParticle[particleIndex].rotation.acceleration = float3(
                            GenerateRandomInRange(generator.Generate1d(), gEffectNode[nodeIndex].rotation.pva.acceleration.x.median, gEffectNode[nodeIndex].rotation.pva.acceleration.x.amplitude),
                            GenerateRandomInRange(generator.Generate1d(), gEffectNode[nodeIndex].rotation.pva.acceleration.y.median, gEffectNode[nodeIndex].rotation.pva.acceleration.y.amplitude),
                            GenerateRandomInRange(generator.Generate1d(), gEffectNode[nodeIndex].rotation.pva.acceleration.z.median, gEffectNode[nodeIndex].rotation.pva.acceleration.z.amplitude)
                        );
                case SRT_TYPE_EASING:
                    break;
            }
                // Scale
            switch (gEffectNode[nodeIndex].scale.type)
            {
                case SRT_TYPE_STANDARD:
                    gEffectParticle[particleIndex].scale.value = gEffectNode[nodeIndex].scale.value;
                    break;
                case SRT_TYPE_PVA:
                    gEffectParticle[particleIndex].scale.value = float3(
                            GenerateRandomInRange(generator.Generate1d(), gEffectNode[nodeIndex].scale.pva.value.x.median, gEffectNode[nodeIndex].scale.pva.value.x.amplitude),
                            GenerateRandomInRange(generator.Generate1d(), gEffectNode[nodeIndex].scale.pva.value.y.median, gEffectNode[nodeIndex].scale.pva.value.y.amplitude),
                            GenerateRandomInRange(generator.Generate1d(), gEffectNode[nodeIndex].scale.pva.value.z.median, gEffectNode[nodeIndex].scale.pva.value.z.amplitude)
                        );
                    gEffectParticle[particleIndex].scale.velocity = float3(
                            GenerateRandomInRange(generator.Generate1d(), gEffectNode[nodeIndex].scale.pva.velocity.x.median, gEffectNode[nodeIndex].scale.pva.velocity.x.amplitude),
                            GenerateRandomInRange(generator.Generate1d(), gEffectNode[nodeIndex].scale.pva.velocity.y.median, gEffectNode[nodeIndex].scale.pva.velocity.y.amplitude),
                            GenerateRandomInRange(generator.Generate1d(), gEffectNode[nodeIndex].scale.pva.velocity.z.median, gEffectNode[nodeIndex].scale.pva.velocity.z.amplitude)
                        );
                    gEffectParticle[particleIndex].scale.acceleration = float3(
                            GenerateRandomInRange(generator.Generate1d(), gEffectNode[nodeIndex].scale.pva.acceleration.x.median, gEffectNode[nodeIndex].scale.pva.acceleration.x.amplitude),
                            GenerateRandomInRange(generator.Generate1d(), gEffectNode[nodeIndex].scale.pva.acceleration.y.median, gEffectNode[nodeIndex].scale.pva.acceleration.y.amplitude),
                            GenerateRandomInRange(generator.Generate1d(), gEffectNode[nodeIndex].scale.pva.acceleration.z.median, gEffectNode[nodeIndex].scale.pva.acceleration.z.amplitude)
                        );
                case SRT_TYPE_EASING:
                    break;
            }
                // Color
            switch (gEffectMesh[meshIndex].colorType)
            {
                case COLOR_TYPE_CONSTANT:
                    gEffectParticle[particleIndex].color = gEffectMesh[meshIndex].color;
                    break;
                case COLOR_TYPE_RANDOM:
                    gEffectParticle[particleIndex].color = float4(
                            generator.GenerateInRange(gEffectMesh[meshIndex].randColor.minColor.r, gEffectMesh[meshIndex].randColor.maxColor.r),
                            generator.GenerateInRange(gEffectMesh[meshIndex].randColor.minColor.g, gEffectMesh[meshIndex].randColor.maxColor.g),
                            generator.GenerateInRange(gEffectMesh[meshIndex].randColor.minColor.b, gEffectMesh[meshIndex].randColor.maxColor.b),
                            generator.GenerateInRange(gEffectMesh[meshIndex].randColor.minColor.a, gEffectMesh[meshIndex].randColor.maxColor.a)
                        );
                    break;
                case COLOR_TYPE_EASING:
                    gEffectParticle[particleIndex].color = float4(
                            generator.GenerateInRange(gEffectMesh[meshIndex].easingColor.startMinColor.r, gEffectMesh[meshIndex].easingColor.startMaxColor.r),
                            generator.GenerateInRange(gEffectMesh[meshIndex].easingColor.startMinColor.g, gEffectMesh[meshIndex].easingColor.startMaxColor.g),
                            generator.GenerateInRange(gEffectMesh[meshIndex].easingColor.startMinColor.b, gEffectMesh[meshIndex].easingColor.startMaxColor.b),
                            generator.GenerateInRange(gEffectMesh[meshIndex].easingColor.startMinColor.a, gEffectMesh[meshIndex].easingColor.startMaxColor.a)
                        );
                    break;
            }
                // LifeTime
            gEffectParticle[particleIndex].lifeTime = GenerateRandomInRange(generator.Generate1d(), gEffectNode[nodeIndex].common.lifeTime.median, gEffectNode[nodeIndex].common.lifeTime.amplitude);
                
            gEffectParticle[particleIndex].currentTime = 0.0f;
            gEffectParticle[particleIndex].isAlive = 1;
            gEffectParticle[particleIndex].nodeID = nodeIndex;
            gEffectParticle[particleIndex].meshID = meshIndex;
        }

    }

}