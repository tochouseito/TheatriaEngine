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
    float4 albedo : SV_Target0; //アルベド
    float4 normal : SV_Target1; // 法線
    float4 position : SV_Target2; // 位置
    float4 material : SV_Target3; // マテリアル
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    
    // テクスチャ
    float4 transformedUV = mul(float4(input.texcoord, 0.0f, 1.0f), gMaterial.matUV);
    float4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
    
    // アルベドカラーを出力
    output.albedo = gMaterial.color * textureColor;
    
    // 法線を出力
    // 出力は0～1に丸められてしまうのでマイナスの値が失われてしまう
    // なので-1～1を0～1に変換する
    // (-1 ～ 1) ÷ 2.0       = (-0.5 ～ 0.5)
    // (-0.5 ～ 0.5) + 0.5  = (0.0 ～ 1.0)
    output.normal.rgb = (input.normal / 2.0f) + 0.5f;
    output.normal.a = 1.0f;
    //output.normal = float4(input.normal,1.0f);
    //output.normal = float4(input.normal, 1.0f);
    
    output.position = float4(input.worldPosition.xyz, 1.0f);
    
    output.material = float4(0.0f, 0.0f, gMaterial.shininess, float(gMaterial.enableLighting));

    return output;
}