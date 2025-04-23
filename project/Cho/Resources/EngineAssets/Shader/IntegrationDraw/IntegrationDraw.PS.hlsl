#include "../header/Demo.hlsli"

// マテリアル
StructuredBuffer<Material> gIMaterial : register(t0);

// テクスチャリソース(カラー)
//Texture2D<float4> gTextures[] : register(t1);
Texture2D<float4> gTexture : register(t1);

// サンプラー
SamplerState gSampler : register(s0);

struct PixelShaderOutput {
    float4 color : SV_TARGET0;
};

PixelShaderOutput main(VSOut input) {
    PixelShaderOutput output;
    
    Material material = gIMaterial[input.materialID];
    float4 textureColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
    if (material.enableTexture != 0)// テクスチャが有効なら
    {
        // テクスチャ
        float4 transformedUV = mul(float4(input.texcoord, 0.0f, 1.0f), gIMaterial[input.materialID].matUV);
        textureColor = gTexture.Sample(gSampler, transformedUV.xy);
    }
        // 合計
    output.color = material.color * textureColor;

    return output;
}