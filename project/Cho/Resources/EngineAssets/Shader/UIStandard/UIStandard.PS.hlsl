#include "../header/UIStandard.hlsli"
#include "../header/Demo.hlsli"
struct Material
{
    float4 color;
    float4x4 matUV;
    float shininess;
    uint enableLighting;
    uint enableTexture;
    uint textureID;
};
// マテリアル
StructuredBuffer<Material> gIMaterial : register(t0);
// テクスチャリソース(カラー)
Texture2D<float4> gTextures[] : register(t1);
// サンプラー
SamplerState gSampler : register(s0);

struct PSOutput
{
    float4 colorPS : SV_TARGET0;
};

PSOutput main(VSOutput input)
{
    PSOutput output;
    
    // マテリアル
    Material material = gIMaterial[input.materialID];
    
    float4 transformedUV = mul(float4(input.texcoord, 0.0f, 1.0f), material.matUV);
    float4 textureColor = gTextures[material.textureID].Sample(gSampler, transformedUV.xy);
    
    if (textureColor.a == 0.0)
    {
        discard;
    }
   
    output.colorPS.rgb = material.color.rgb * textureColor.rgb;
    output.colorPS.a = material.color.a;
    
    return output;
}