#include "../header/Sprite.hlsli"

cbuffer Material : register(b0)
{
    float4 colorMaterial : SV_TARGET0;
    int enableLighting;
    float4x4 matUV;
    float shininess;
};

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct PixelShaderOutput
{
    float4 colorPS : SV_TARGET0;
};

PixelShaderOutput main(SpriteVertexShaderOutput input)
{
    PixelShaderOutput output;
    
    float4 transformedUV = mul(float4(input.texcoord, 0.0f, 1.0f), matUV);
    float4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
    
    if (textureColor.a == 0.0)
    {
        discard;
    }
   
    output.colorPS.rgb = colorMaterial.rgb * textureColor.rgb;
    output.colorPS.a = colorMaterial.a;
    
    return output;
}