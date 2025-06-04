#include "../header/ViewProjection.hlsli"
#include "../header/Skybox.hlsli"

struct VSInput
{
    float4 position : POSITION;
    float2 texCoord : TEXCOORD0;
};

// CBV : ViewProjection
ConstantBuffer<ViewProjection> gVP : register(b0);
// CBV : Transform
ConstantBuffer<SkyboxTransform> gTransform : register(b1);

VSOutput main(VSInput input)
{
    VSOutput output;
    // worldViewProjection
    float4x4 WVP = mul(gVP.view, gVP.projection);
    output.position = mul(input.position, mul(gTransform.worldMatrix, WVP)).xyww;
    output.texCoord = input.position.xyz;
    return output;
}