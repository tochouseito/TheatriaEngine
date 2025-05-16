#include "../header/Demo.hlsli"

// ViewProjectionResource
ConstantBuffer<ViewProjection> gVP : register(b0);
// IntegrationTransformResource
StructuredBuffer<Transform> gITF : register(t0, space0);
// UseTransformMapIndexResource
StructuredBuffer<uint> gUseIndex : register(t1, space0);

struct VSIn {
    float4 position : POSITION;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
    float4 color : COLOR;
    uint vertexID : VERTEXID;
};
VSOut main(VSIn input, uint instanceId : SV_InstanceID) {
    VSOut output;
    
    uint index = gUseIndex[instanceId];
    Transform tf = gITF[index];
    
    // worldViewProjection
    float4x4 WVP = mul(gVP.view, gVP.projection);
    WVP = mul(tf.matWorld, WVP);
    // rootNode適用
    output.position = mul(input.position, mul(tf.rootNode, WVP));
    // texcoord
    output.texcoord = input.texcoord;
    // normal
    output.normal = normalize(mul(input.normal, (float3x3)tf.worldInverse));
    //
    output.worldPosition = mul(input.position, mul(tf.matWorld, tf.rootNode)).xyz;
    //
    output.cameraPosition = gVP.cameraPosition;
    // 
    output.materialID = tf.materialID;

    return output;
}