#include "../header/ViewProjection.hlsli"
//struct World
//{
//    float4x4 matWorld;
//};

//ConstantBuffer<World> gWorld : register(b0);

// ビュープロジェクションリソース
ConstantBuffer<ViewProjection> gVP : register(b0);

struct VSInput
{
    float3 position: POSITION;
    float4 color : COLOR;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

static float4x4 matWorld =
{
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f
};

PSInput main(VSInput input)
{
    PSInput output;
    
    // 頂点インデックスに基づいて開始点または終了点を選択
    float3 position =input.position;
    
    // worldViewProjection
    float4x4 WVP = mul(gVP.view, gVP.projection);
    WVP = mul(matWorld, WVP);
    
    output.position = mul(float4(position, 1.0), WVP);
    output.color = input.color;
    return output;
}