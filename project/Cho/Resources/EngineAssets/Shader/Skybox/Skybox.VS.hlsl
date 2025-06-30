#include "../header/ViewProjection.hlsli"
#include "../header/Skybox.hlsli"
#include "../header/Math.hlsli"

struct VSInput {
    float4 position : POSITION;
    float2 texCoord : TEXCOORD0;
};

// CBV : ViewProjection
ConstantBuffer<ViewProjection> gVP : register(b0);

VSOutput main(VSInput input) {
    VSOutput output;
    // worldViewProjection
    float4x4 WVP = mul(gVP.view, gVP.projection);
    // TranslateMatrix
    float4x4 translateMatrix = TranslateMatrix(gVP.cameraPosition);
    output.position = mul(input.position, mul(translateMatrix, WVP)).xyww;
    output.texCoord = input.position.xyz;
    return output;
}