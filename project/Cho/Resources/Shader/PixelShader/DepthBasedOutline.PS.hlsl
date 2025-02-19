#include "../header/FullScreen.hlsli"
#include "../header/ViewProjection.hlsli"

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

ConstantBuffer<ViewProjection> gVP : register(b0);

Texture2D<float4> gTexure : register(t0);
Texture2D<float> gDepthTexture : register(t1);

SamplerState gSampler : register(s0);
SamplerState gSamplerPoint : register(s1);

static const uint size = 3;
static const float powSize = float(size * size);
static const float kPrewittHorizontalKernel[size][size] =
{
    { -1.0f / 6.0f, 0.0f, 1.0f / 6.0f },
    { -1.0f / 6.0f, 0.0f, 1.0f / 6.0f },
    { -1.0f / 6.0f, 0.0f, 1.0f / 6.0f },
};
static const float kPrewittVerticalKernel[size][size] =
{
    { -1.0f / 6.0f, -1.0f / 6.0f, -1.0f / 6.0f },
    { 0.0f, 0.0f, 0.0f },
    { 1.0f / 6.0f, 1.0f / 6.0f, 1.0f / 6.0f },
};
static const float2 kIndex[size][size] =
{
    { { -1.0f, -1.0f }, { 0.0f, -1.0f }, { 1.0f, -1.0f } },
    { { -1.0f, 0.0f }, { 0.0f, 0.0f }, { 1.0f, 0.0f } },
    { { -1.0f, 1.0f }, { 0.0f, 1.0f }, { 1.0f, 1.0f } },
};
float Luminance(float3 v)
{
    return dot(v, float3(0.2125f, 0.7154f, 0.0721f));
}

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    float2 difference = float2(0.0f, 0.0f); // 縦横それぞれの畳み込みの結果を格納する
    uint width, height; // 1.uvStepSize算出
    gTexure.GetDimensions(width, height);
    float2 uvStepSize = float2(rcp(float(width)), rcp(float(height)));
    float4x4 pi = gVP.projectionInverse;
    // 色を輝度に変換して、畳み込みを行っていく。微分Filter用のkernelになっているので、やること自体は今までの畳み込みと同じ
    for (int x = 0; x < size; ++x)
    {
        for (int y = 0; y < size; ++y)
        {
            float2 texcoord = input.texcoord + kIndex[x][y] * uvStepSize;
            float ndcDepth = gDepthTexture.Sample(gSamplerPoint, texcoord);
            // NDC->View。P^{-1}においてxとyはzwに影響を与えないので何でもいい。なので、わざわざ行列を渡さなくてもいい
            // gMaterial.projectionInverseはCBufferを使って渡して置くこと
            float4 viewSpace = mul(float4(0.0f, 0.0f, ndcDepth, 1.0f), pi);
            float viewZ = viewSpace.z * rcp(viewSpace.w);
            difference.x += viewZ * kPrewittHorizontalKernel[x][y];
            difference.y += viewZ * kPrewittVerticalKernel[x][y];
        }
    }
    // 変化の長さをウェイトとして合成。ウェイトの決定方法もいろいろと考えられる。たとえばdifference.xだけを使えば横方向のエッジが検出される
    float weight = length(difference);
    // 差が小さすぎて分かりづらいので適当に倍している。CBufferで調整パラメータとして送ったりするといい
    weight = saturate(weight);
    // weightが大きいほど暗く表示するようにしている。最もシンプルな合成方法
    output.color.rgb = (1.0f - weight) * gTexure.Sample(gSampler, input.texcoord).rgb;
    output.color.a = 1.0f;
    return output;
}