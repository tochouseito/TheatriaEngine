#include "../header/Particle.hlsli"
#include "../header/Random.hlsli"
#include "../header/Effect.hlsli"

class RandomGenerator
{
    float3 seed;
    float3 Generate3d()
    {
        seed = rand3dTo3d(seed);
        return seed;
    }
    float Generate1d()
    {
        float result = rand3dTo1d(seed);
        seed.x = result;
        return result;
    }
};

static const uint kMaxParticles = 256;

// CBV: タイムマネージャー
ConstantBuffer<TimeManager> gTimeManager : register(b0);

// UAV: エフェクトノード
StructuredBuffer<EffectNode> gEffectNode:register(t0);

// UAV: パーティクル
RWStructuredBuffer<EffectParticle> gParticles : register(u0);

// UAV: フリーリストのIndex
RWStructuredBuffer<int> gFreeListIndex : register(u1);

// UAV: フリーリスト
RWStructuredBuffer<uint> gFreeList : register(u2);

[numthreads(1, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    // シード生成
    RandomGenerator generator;
    generator.seed = (DTid + gTimeManager.globalTime) * gTimeManager.globalTime;
    // 発生処理
    int emitCount = 0;// 発生開始から、発生の間隔時間
    // 発生開始時間を設定（ランダムではない）
    float emitStartTime= gEffectNode[0].common.emitStartTime.randValue.first;
    // 経過時間が発生開始時間を超えてから発生開始
    if (gTimeManager.globalTime >= emitStartTime)
    {
        // 経過時間から発生開始時間を引いて、はせ
        emitCount = gTimeManager.globalTime - emitStartTime;
        uint emit;// 発生するかどうか
        if (emitCount == 0)
        {
            emit = 0;
        }
        else
        {
            emit = emitCount % GenerateRandomInRange(generator.Generate1d(), gEffectNode[0].common.emitTime.randValue.first, gEffectNode[0].common.emitTime.randValue.second);
        }
        
        if (emit == 0)
        {
            // 発生
            for (uint countIndex = 0; countIndex < gEffectNode[0].common.emitCount; ++countIndex)
            {
                // 各パーティクルごとにシードを変える
                generator.seed = generator.Generate3d() + float3(countIndex, countIndex * 2, countIndex * 3);
                
                int freeListIndex;
                // freeListのindexを一つ前に設定し、現在のIndexを取得する
                InterlockedAdd(gFreeListIndex[0], -1, freeListIndex);
                if (0 <= freeListIndex && freeListIndex < kMaxParticles)
                {
                    // パーティクル初期化
                    uint particleIndex = gFreeList[freeListIndex];
                    
                    // スケール
                    if (gEffectNode[0].scale.type == 0)
                    {
                        gParticles[particleIndex].scale.value = gEffectNode[0].scale.value;
                    }
                    else
                    {
                        gParticles[particleIndex].scale.value = float3(
                            GenerateRandomInRange(generator.Generate1d(), gEffectNode[0].scale.pva.value.first.x, gEffectNode[0].scale.pva.value.second.x),
                            GenerateRandomInRange(generator.Generate1d(), gEffectNode[0].scale.pva.value.first.y, gEffectNode[0].scale.pva.value.second.y),
                            GenerateRandomInRange(generator.Generate1d(), gEffectNode[0].scale.pva.value.first.z, gEffectNode[0].scale.pva.value.second.z)
                        );
                        gParticles[particleIndex].scale.velocity = float3(
                            GenerateRandomInRange(generator.Generate1d(), gEffectNode[0].scale.pva.velocity.first.x, gEffectNode[0].scale.pva.velocity.second.x),
                            GenerateRandomInRange(generator.Generate1d(), gEffectNode[0].scale.pva.velocity.first.y, gEffectNode[0].scale.pva.velocity.second.y),
                            GenerateRandomInRange(generator.Generate1d(), gEffectNode[0].scale.pva.velocity.first.z, gEffectNode[0].scale.pva.velocity.second.z)
                        );
                        gParticles[particleIndex].scale.acceleration = float3(
                            GenerateRandomInRange(generator.Generate1d(), gEffectNode[0].scale.pva.acceleration.first.x, gEffectNode[0].scale.pva.acceleration.second.x),
                            GenerateRandomInRange(generator.Generate1d(), gEffectNode[0].scale.pva.acceleration.first.y, gEffectNode[0].scale.pva.acceleration.second.y),
                            GenerateRandomInRange(generator.Generate1d(), gEffectNode[0].scale.pva.acceleration.first.z, gEffectNode[0].scale.pva.acceleration.second.z)
                        );
                    }
                    
                    // 位置
                    if (gEffectNode[0].position.type == 0)
                    {
                        gParticles[particleIndex].position.value = gEffectNode[0].position.value;
                    }
                    else
                    {
                        gParticles[particleIndex].position.value = float3(
                            GenerateRandomInRange(generator.Generate1d(), gEffectNode[0].position.pva.value.first.x, gEffectNode[0].position.pva.value.second.x),
                            GenerateRandomInRange(generator.Generate1d(), gEffectNode[0].position.pva.value.first.y, gEffectNode[0].position.pva.value.second.y),
                            GenerateRandomInRange(generator.Generate1d(), gEffectNode[0].position.pva.value.first.z, gEffectNode[0].position.pva.value.second.z)
                        );
                        gParticles[particleIndex].position.velocity = float3(
                            GenerateRandomInRange(generator.Generate1d(), gEffectNode[0].position.pva.velocity.first.x, gEffectNode[0].position.pva.velocity.second.x),
                            GenerateRandomInRange(generator.Generate1d(), gEffectNode[0].position.pva.velocity.first.y, gEffectNode[0].position.pva.velocity.second.y),
                            GenerateRandomInRange(generator.Generate1d(), gEffectNode[0].position.pva.velocity.first.z, gEffectNode[0].position.pva.velocity.second.z)
                        );
                        gParticles[particleIndex].position.acceleration = float3(
                            GenerateRandomInRange(generator.Generate1d(), gEffectNode[0].position.pva.acceleration.first.x, gEffectNode[0].position.pva.acceleration.second.x),
                            GenerateRandomInRange(generator.Generate1d(), gEffectNode[0].position.pva.acceleration.first.y, gEffectNode[0].position.pva.acceleration.second.y),
                            GenerateRandomInRange(generator.Generate1d(), gEffectNode[0].position.pva.acceleration.first.z, gEffectNode[0].position.pva.acceleration.second.z)
                        );
                    }
                    
                    // 回転
                    if (gEffectNode[0].rotation.type == 0)
                    {
                        gParticles[particleIndex].rotation.value = gEffectNode[0].rotation.value;
                    }
                    else
                    {
                        gParticles[particleIndex].rotation.value = float3(
                            GenerateRandomInRange(generator.Generate1d(), gEffectNode[0].rotation.pva.value.first.x, gEffectNode[0].rotation.pva.value.second.x),
                            GenerateRandomInRange(generator.Generate1d(), gEffectNode[0].rotation.pva.value.first.y, gEffectNode[0].rotation.pva.value.second.y),
                            GenerateRandomInRange(generator.Generate1d(), gEffectNode[0].rotation.pva.value.first.z, gEffectNode[0].rotation.pva.value.second.z)
                        );
                        gParticles[particleIndex].rotation.velocity = float3(
                            GenerateRandomInRange(generator.Generate1d(), gEffectNode[0].rotation.pva.velocity.first.x, gEffectNode[0].rotation.pva.velocity.second.x),
                            GenerateRandomInRange(generator.Generate1d(), gEffectNode[0].rotation.pva.velocity.first.y, gEffectNode[0].rotation.pva.velocity.second.y),
                            GenerateRandomInRange(generator.Generate1d(), gEffectNode[0].rotation.pva.velocity.first.z, gEffectNode[0].rotation.pva.velocity.second.z)
                        );
                        gParticles[particleIndex].rotation.acceleration = float3(
                            GenerateRandomInRange(generator.Generate1d(), gEffectNode[0].rotation.pva.acceleration.first.x, gEffectNode[0].rotation.pva.acceleration.second.x),
                            GenerateRandomInRange(generator.Generate1d(), gEffectNode[0].rotation.pva.acceleration.first.y, gEffectNode[0].rotation.pva.acceleration.second.y),
                            GenerateRandomInRange(generator.Generate1d(), gEffectNode[0].rotation.pva.acceleration.first.z, gEffectNode[0].rotation.pva.acceleration.second.z)
                        );
                    }
                    
                    gParticles[particleIndex].color.rgb = gEffectNode[0].draw.meshSprite.color.rgb;
                    gParticles[particleIndex].color.a = 1.0f;
                    
                    gParticles[particleIndex].lifeTime = GenerateRandomInRange(generator.Generate1d(), gEffectNode[0].common.lifeTime.randValue.first, gEffectNode[0].common.lifeTime.randValue.second);
                    
                    gParticles[particleIndex].currentTime = 0.0f;
                    gParticles[particleIndex].isAlive = true;
                }
                else
                {
                    // 発生させられなかったので、減らしてしまった分元に戻す。これを忘れると発生させられなかった分だけIndexが減ってしまう
                    InterlockedAdd(gFreeListIndex[0], 1);
                    // Emit中にParticleは消えないので、この後発生することはないためBreakして終わらせる
                    break;
                }
            }
        }
    }
}