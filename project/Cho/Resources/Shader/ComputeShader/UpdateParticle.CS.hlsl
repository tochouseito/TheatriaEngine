#include "../header/Particle.hlsli"
static const uint kMaxParticles = 1024;

RWStructuredBuffer<Particle> gParticles : register(u0);
ConstantBuffer<PerFrame> gPerFrame : register(b0);
RWStructuredBuffer<int> gFreeListIndex : register(u1);
RWStructuredBuffer<uint> gFreeList : register(u2);

[numthreads(1024, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    uint particleIndex = DTid.x;
    if (particleIndex < kMaxParticles)
    {
        // alphaが0のparticleは死んでいるとみなして更新しない
        if (gParticles[particleIndex].color.a != 0.0f)
        {
            gParticles[particleIndex].translate += gParticles[particleIndex].velocity;
            gParticles[particleIndex].currentTime += gPerFrame.deltaTime;
            float alpha = 1.0f - (gParticles[particleIndex].currentTime / gParticles[particleIndex].lifeTime);
            gParticles[particleIndex].color.a = saturate(alpha);
        }
        // alphaが0になったのでここはフリーとする
        if (gParticles[particleIndex].isAlive && gParticles[particleIndex].color.a <= 0.0f)
        {
            // スケールに0を入れておいてVertexShader出力で棄却されるようにする
            gParticles[particleIndex].scale = float3(0.0f, 0.0f, 0.0f);
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