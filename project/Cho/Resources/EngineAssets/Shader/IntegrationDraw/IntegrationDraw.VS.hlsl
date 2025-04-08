#include "../header/Demo.hlsli"

// ViewProjectionResource
ConstantBuffer<ViewProjection> gVP : register(b0);
// IntegrationTransformResource
StructuredBuffer<Transform> gITF : register(t0);
// UseTransformMapIndexResource
StructuredBuffer<uint> gUseIndex : register(t1);

struct VSIn {
    float4 position : POSITION;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
};
VSOut main(VSIn input, uint instanceId : SV_InstanceID) {
    VSOut output;
    
    // worldViewProjection
    float4x4 WVP = mul(gVP.view, gVP.projection);
    WVP = mul(gITF[gUseIndex[instanceId]].matWorld, WVP);
    // rootNode適用
    output.position = mul(input.position, mul(gITF[gUseIndex[instanceId]].rootNode, WVP));
    // texcoord
    output.texcoord = input.texcoord;
    // normal
    output.normal = normalize(mul(input.normal, (float3x3) gITF[gUseIndex[instanceId]].worldInverse));
    //
    output.worldPosition = mul(input.position, mul(gITF[gUseIndex[instanceId]].matWorld, gITF[gUseIndex[instanceId]].rootNode)).xyz;
    //
    output.cameraPosition = gVP.cameraPosition;

    return output;
}