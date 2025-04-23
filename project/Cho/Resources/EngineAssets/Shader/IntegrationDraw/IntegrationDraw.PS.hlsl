#include "../header/Demo.hlsli"
#include "../header/Material.hlsli"

// マテリアル
//StructuredBuffer<Material> gIMaterial : register(t0);

// テクスチャリソース(カラー)
//Texture2D<float4> gTextures[] : register(t1);
Texture2D<float4> gTexture : register(t0);

// サンプラー
SamplerState gSampler : register(s0);

struct PixelShaderOutput {
    float4 color : SV_TARGET0;
};

PixelShaderOutput main(VSOut input) {
    PixelShaderOutput output;
    
    //float4 textureColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
    
    //if (gIMaterial[input.materialID].enableTexture != 0) {
    //    // テクスチャ
    //    float4 transformedUV = mul(float4(input.texcoord, 0.0f, 1.0f), gIMaterial[input.materialID].matUV);
    //    textureColor = gTextures[gIMaterial[input.materialID].textureID].Sample(gSampler, transformedUV.xy);
    //}
    
    //// 合計
    //output.color = gIMaterial[input.materialID].color * textureColor;
    output.color = gTexture.Sample(gSampler, input.texcoord);

    return output;
}