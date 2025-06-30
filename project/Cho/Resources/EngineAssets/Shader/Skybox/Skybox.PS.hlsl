#include "../header/Skybox.hlsli"

TextureCube<float4> gTextureCube : register(t0);
SamplerState gSampler : register(s0);

struct PSOutput {
    float4 color : SV_Target0;
};

PSOutput main(VSOutput input) {
    PSOutput output;

    float4 textureColor = gTextureCube.Sample(gSampler, input.texCoord);
    output.color = textureColor;
    return output;
}