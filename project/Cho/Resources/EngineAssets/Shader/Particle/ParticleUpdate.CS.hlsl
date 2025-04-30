#include "../header/Particle.hlsli"

static const uint kMaxParticles = 1024;

// UAV: パーティクルリソース
RWStructuredBuffer<Particle> gParticles : register(u0);
// CBV: PerFrameリソース
ConstantBuffer<PerFrame> gPerFrame : register(b0);
// UAV: フリーリスト
AppendStructuredBuffer<uint> gFreeList : register(u1);

[numthreads(1024, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
    uint particleIndex = DTid.x;
    if (particleIndex < kMaxParticles) {
        if (gParticles[particleIndex].color.a != 0.0f) {
            gParticles[particleIndex].position.value += gParticles[particleIndex].position.velocity;
            gParticles[particleIndex].currentTime += gPerFrame.deltaTime;
            float alpha = 1.0f - (gParticles[particleIndex].currentTime / gParticles[particleIndex].lifeTime);
            gParticles[particleIndex].color.a = saturate(alpha);
        }

        if (gParticles[particleIndex].isAlive && gParticles[particleIndex].color.a <= 0.0f) {
            gParticles[particleIndex].scale.value = float3(0.0f, 0.0f, 0.0f);
            gParticles[particleIndex].isAlive = false;
            gFreeList.Append(particleIndex); // 死んだらAppend
        }
    }
    
}
