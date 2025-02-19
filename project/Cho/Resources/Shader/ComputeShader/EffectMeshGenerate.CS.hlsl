#include "../header/Effect.hlsli"

// SRV: EffectNodeバッファ
StructuredBuffer<EffectNode> effectNode : register(t0);

// UAV: 頂点バッファ
RWStructuredBuffer<VertexData> vertices : register(u0);

// UAV: インデックスバッファ
RWStructuredBuffer<uint> indices : register(u1);

[numthreads(1, 1, 1)]
void main(uint3 dispatchID : SV_DispatchThreadID)
{
    // メッシュ生成
    if (effectNode[0].draw.meshType == 1)
    {
        
        // 頂点データ（重複なし）
        // 右上
        vertices[0].position = float4(1.0f, 1.0f, 0.0f, 1.0f);
        vertices[0].texcoord = float2(1.0f, 0.0f);
        vertices[0].normal = float3(0.0f, 0.0f, 1.0f);
    
        // 左上
        vertices[1].position = float4(-1.0f, 1.0f, 0.0f, 1.0f);
        vertices[1].texcoord = float2(0.0f, 0.0f);
        vertices[1].normal = float3(0.0f, 0.0f, 1.0f);
    
        // 右下
        vertices[2].position = float4(1.0f, -1.0f, 0.0f, 1.0f);
        vertices[2].texcoord = float2(1.0f, 1.0f);
        vertices[2].normal = float3(0.0f, 0.0f, 1.0f);
    
        // 左下
        vertices[3].position = float4(-1.0f, -1.0f, 0.0f, 1.0f);
        vertices[3].texcoord = float2(0.0f, 1.0f);
        vertices[3].normal = float3(0.0f, 0.0f, 1.0f);
    
        // インデックスデータ
        indices[0] = 0;
        indices[1] = 1;
        indices[2] = 2;
        indices[3] = 1;
        indices[4] = 3;
        indices[5] = 2;
    }
}
