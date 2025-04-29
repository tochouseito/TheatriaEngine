#include "../header/Particle.hlsli"
#include "../header/Random.hlsli"

static const uint kMaxParticles = 1024;

// UAV: パーティクル
RWStructuredBuffer<Particle> gParticles : register(u0);
// CBV: エミッター
ConstantBuffer<EmitterSphere> gEmitter : register(b0);
ConstantBuffer<PerFrame> gPerFrame : register(b1);
// UAV: フリーリスト
ConsumeStructuredBuffer<uint> gFreeList : register(u1);

[numthreads(1, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
    if (gEmitter.emit != 0) {
        RandomGenerator generator;
        generator.seed = (DTid + gPerFrame.time) * gPerFrame.time;

        for (uint countIndex = 0; countIndex < gEmitter.count; ++countIndex) {
            // 戻り値で取得する
            uint particleIndex = gFreeList.Consume();

            // Append/Consume は空の時に自動でフェイルしないので、indexチェックが必要（オプション）
            if (particleIndex < kMaxParticles) {
                gParticles[particleIndex].scale.value = float3(0.5f, 0.5f, 0.5f);
                gParticles[particleIndex].position.value = gEmitter.translate;
                gParticles[particleIndex].color.rgb = float3(1.0f, 1.0f, 1.0f);
                gParticles[particleIndex].color.a = 1.0f;
                gParticles[particleIndex].position.velocity = normalize(generator.Generate3d()) * 0.05f;
                gParticles[particleIndex].lifeTime = 0.2f;
                gParticles[particleIndex].currentTime = 0.0f;
                gParticles[particleIndex].isAlive = true;
            }
            else {
                break; // 空きなし
            }
        }
    }
}
