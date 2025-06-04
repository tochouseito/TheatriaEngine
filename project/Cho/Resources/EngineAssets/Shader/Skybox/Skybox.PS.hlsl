#include "../header/Skybox.hlsli"



PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    float32_t4 textureColor = gTexture.Sample(gSampler, input.texcoord);
    output.colorPS = textureColor * colorMaterial;
    return output;
}