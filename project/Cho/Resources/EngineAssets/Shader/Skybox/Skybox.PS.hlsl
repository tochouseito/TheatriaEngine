#include "../header/Skybox.hlsli"

TextureCube<float4> gTextureCube : register(t0);
SamplerState gSampler : register(s0);

struct PSPOutput {
    float4 color : SV_Target0;
};

PSPOutput main(VSOutput input)
{
    PSPOutput output;
    float4 textureColor = gTextureCube.Sample(gSampler, input.texCoord);
    output.color = textureColor;
    return output;
}