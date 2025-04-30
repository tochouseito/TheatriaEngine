#include "../header/Effect.hlsli"

static const uint kMaxParticles = 1024;

// UAV : EffectRoot
RWStructuredBuffer<EffectRoot> gEffectRoot : register(u0);
// UAV : EffectNode
RWStructuredBuffer<EffectNode> gEffectNode : register(u1);
// UAV : EffectMeshData
RWStructuredBuffer<EffectSprite> gEffectMeshData : register(u2);
// UAV : EffectParticle
AppendStructuredBuffer<EffectParticle> gEffectParticle : register(u3);

[numthreads(kMaxParticles, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID, uint3 Gid : SV_GroupID,uint3 GTid : SV_GroupThreadID) {
    
}