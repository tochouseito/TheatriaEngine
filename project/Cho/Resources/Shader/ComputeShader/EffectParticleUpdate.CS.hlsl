#include "../header/Particle.hlsli"
#include "../header/Effect.hlsli"
static const uint kMaxParticles = 256;

// CBV: タイムマネージャー
ConstantBuffer<TimeManager> gTimeManager : register(b0);

// SRV: エフェクトノード
StructuredBuffer<EffectNode> gEffectNode:register(t0);

// UAV: パーティクル
RWStructuredBuffer<EffectParticle> gParticles : register(u0);

// UAV: フリーリストのIndex
RWStructuredBuffer<int> gFreeListIndex : register(u1);

// UAV: フリーリスト
RWStructuredBuffer<uint> gFreeList : register(u2);

[numthreads(256, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    uint particleIndex = DTid.x;
    if (particleIndex < kMaxParticles)
    {
        // alphaが0のparticleは死んでいるとみなして更新しない
        if (gParticles[particleIndex].color.a != 0.0f)
        {
            // 位置を更新
            gParticles[particleIndex].position.value += gParticles[particleIndex].position.velocity;
            // 速度を更新
            gParticles[particleIndex].position.velocity += gParticles[particleIndex].position.acceleration;
            
            // スケールを更新
            gParticles[particleIndex].scale.value += gParticles[particleIndex].scale.velocity;
            // 速度を更新
            gParticles[particleIndex].scale.velocity += gParticles[particleIndex].scale.acceleration;
            
            // 回転を更新
            gParticles[particleIndex].rotation.value += gParticles[particleIndex].rotation.velocity;
            // 速度を更新
            gParticles[particleIndex].rotation.velocity += gParticles[particleIndex].rotation.acceleration;
            
            // 色を更新
            if (gEffectNode[0].drawCommon.fadeOutType == 0)
            {
                float lifeTime = 1.0f - (gParticles[particleIndex].currentTime / gParticles[particleIndex].lifeTime);
                lifeTime = saturate(lifeTime);
                if (lifeTime <= 0.0f)
                {
                    gParticles[particleIndex].color.a = 0.0f;
                }
            }
            else
            {
                float alpha = 1.0f - (gParticles[particleIndex].currentTime / gParticles[particleIndex].lifeTime);
                gParticles[particleIndex].color.a = saturate(alpha);
            }
            
            
            // 経過時間を更新
            gParticles[particleIndex].currentTime++;
        }
        // alphaが0になったのでここはフリーとする
        if (gParticles[particleIndex].isAlive && gParticles[particleIndex].color.a <= 0.0f)
        {
            // スケールに0を入れておいてVertexShader出力で棄却されるようにする
            gParticles[particleIndex].scale.value = float3(0.0f, 0.0f, 0.0f);
            gParticles[particleIndex].isAlive = false;
            int freeListIndex;
            InterlockedAdd(gFreeListIndex[0], 1, freeListIndex);
            // 最新のfreeListIndexの場所に死んだParticleのIndexを設定する
            if ((freeListIndex + 1) < kMaxParticles)
            {
                gFreeList[freeListIndex + 1] = particleIndex;
            }
            else
            {
                // ここに来るはずがない、来たら何かが間違っているが、安全策をうつ
                InterlockedAdd(gFreeListIndex[0], -1, freeListIndex);
            }
        }
    }
}