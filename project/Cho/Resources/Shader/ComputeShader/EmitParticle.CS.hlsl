#include "../header/Particle.hlsli"
#include "../header/Random.hlsli"
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
static const uint kMaxParticles = 1024;
RWStructuredBuffer<Particle> gParticles : register(u0);
ConstantBuffer<EmitterSphere> gEmitter : register(b0);
ConstantBuffer<PerFrame> gPerFrame : register(b1);
RWStructuredBuffer<int> gFreeListIndex : register(u1);
RWStructuredBuffer<uint> gFreeList : register(u2);

[numthreads(1, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    
    if (gEmitter.emit != 0)
    {
        RandomGenerator generator;
        generator.seed = (DTid + gPerFrame.time) * gPerFrame.time;
        // Generate3d呼ぶたびにseedが変わるので結果すべての乱数が変わる
        for (uint countIndex = 0; countIndex < gEmitter.count; ++countIndex)
        {
            int freeListIndex;
            // freeListのindexを一つ前に設定し、現在のIndexを取得する
            InterlockedAdd(gFreeListIndex[0], -1, freeListIndex);
            if (0 <= freeListIndex && freeListIndex < kMaxParticles)
            {
                uint particleIndex = gFreeList[freeListIndex];
                
                
                gParticles[particleIndex].scale = float3(0.5f, 0.5f, 0.5f);
                gParticles[particleIndex].translate = gEmitter.translate;
                gParticles[particleIndex].color.rgb = float3(1.0f, 1.0f, 1.0f);
                gParticles[particleIndex].color.a = 1.0f;
                gParticles[particleIndex].velocity = normalize(generator.Generate3d()) * 0.05f;
                gParticles[particleIndex].lifeTime = 0.2f;
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