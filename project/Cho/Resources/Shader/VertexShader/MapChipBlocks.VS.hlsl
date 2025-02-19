#include "../header/Demo.hlsli"
#include "../header/ViewProjection.hlsli"

// トランスフォームリソース
StructuredBuffer<Transform> gTF : register(t0);

// ビュープロジェクションリソース
ConstantBuffer<ViewProjection> gVP : register(b0);

struct VertexShaderInput
{
    float4 position : POSITION;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
};
VertexShaderOutput main(VertexShaderInput input, uint instanceId : SV_InstanceID)
{
    VertexShaderOutput output;
    
    // worldViewProjection
    float4x4 WVP = mul(gVP.view, gVP.projection);
    WVP = mul(gTF[instanceId].matWorld, WVP);
    
    // rootNode適用
    output.position = mul(input.position, mul(gTF[instanceId].rootNode, WVP));

    // texcoord
    output.texcoord = input.texcoord;
    
    // 法線
    output.normal = normalize(mul(input.normal, (float3x3) gTF[instanceId].worldInverse));
    
    output.worldPosition = mul(input.position, mul(gTF[instanceId].matWorld, gTF[instanceId].rootNode)).xyz;
    
    output.cameraPosition = gVP.cameraPosition;

    return output;
}