#include "../header/Demo.hlsli"
#include "../header/Material.hlsli"

// マテリアル
ConstantBuffer<Material> gMaterial : register(b0);

// テクスチャリソース(カラー)
Texture2D<float4> gTexture : register(t0);

// サンプラー
SamplerState gSampler : register(s0);

struct PixelShaderOutput
{
    float4 pixel : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    
    // テクスチャ
    float4 transformedUV = mul(float4(input.texcoord, 0.0f, 1.0f), gMaterial.matUV);
    float4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
    
    output.pixel = gMaterial.color * textureColor;

    return output;
}