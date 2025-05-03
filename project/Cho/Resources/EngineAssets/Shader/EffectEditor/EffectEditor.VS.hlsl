#include "../header/Effect.hlsli"
#include "../header/Billboard.hlsli"
#include "../header/ViewProjection.hlsli"

// CBV: ビュープロジェクションリソース
ConstantBuffer<ViewProjection> gVP : register(b0);
// CBV : EffectRoot
ConstantBuffer<EffectRoot> gEffectRoot : register(b1);
// SRV : EffectNode
StructuredBuffer<EffectNode> gEffectNode : register(t0);
// SRV : EffectMeshData
StructuredBuffer<EffectSprite> gEffectMesh : register(t1);

// UAV : EffectParticle 128x1024個
RWStructuredBuffer<EffectParticle> gEffectParticle : register(u0);

VSOutput main(VSInput input, uint instanceId : SV_InstanceID){
    VSOutput output;
    
    EffectParticle particle = gEffectParticle[instanceId];
    
    // worldViewProjection
    float4x4 WVP = mul(gVP.view, gVP.projection);
    // cameraMatrix
    float4x4 cameraMatrix = gVP.matWorld;
    // cameraPosition
    float3 cameraPosition = gVP.cameraPosition;
    // scaleMatrix
    float4x4 scaleMatrix = ScaleMatrix(particle.scale.value);
    // translateMatrix
    float4x4 translateMatrix = TranslateMatrix(particle.position.value);
    // rotationMatrix
    
    // billboardMatrix
    float4x4 billboardMatrix = BillboardMatrix(cameraMatrix);
    // worldMatrix
    float4x4 worldMatrix = mul(mul(scaleMatrix, billboardMatrix), translateMatrix);
    worldMatrix[0] *= particle.scale.value.x;
    worldMatrix[1] *= particle.scale.value.y;
    worldMatrix[2] *= particle.scale.value.z;
    worldMatrix[3].xyz = particle.position.value;
    
    output.position = mul(input.position, mul(worldMatrix, WVP));
    output.texcoord = input.texcoord;
    output.normal = input.normal;
    output.color = particle.color;
    
    return output;
}