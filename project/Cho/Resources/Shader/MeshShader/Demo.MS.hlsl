#include "../header/Demo.hlsli"
#include "../header/ViewProjection.hlsli"

///////////////////////////////////////////////////////////////////////////////
// MSInput structure
///////////////////////////////////////////////////////////////////////////////
struct MSInput
{
    float4 position : POSITION;
    float2 texcoord : TEXCOORD;
    float3 normal : NORMAL;
};

///////////////////////////////////////////////////////////////////////////////
// MSOutput structure
///////////////////////////////////////////////////////////////////////////////
struct MSOutput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
    float4 Color : COLOR; // 頂点カラーです.
};

///////////////////////////////////////////////////////////////////////////////
// Meshlet structure
///////////////////////////////////////////////////////////////////////////////
struct Meshlet
{
    uint VertexOffset; // 頂点番号オフセット.
    uint VertexCount; // 出力頂点数.
    uint PrimitiveOffset; // プリミティブ番号オフセット.
    uint PrimitiveCount; // 出力プリミティブ数.
};

//-----------------------------------------------------------------------------
// Resources
//-----------------------------------------------------------------------------
StructuredBuffer<MSInput> Vertices : register(t0);
StructuredBuffer<uint> Indices : register(t1);
ConstantBuffer<Transform> gTF : register(b0);
ConstantBuffer<ViewProjection> gVP : register(b1);

//-----------------------------------------------------------------------------
//      メッシュシェーダのエントリーポイントです.
//-----------------------------------------------------------------------------
[numthreads(64, 1, 1)]
[outputtopology("triangle")]
void main
(
    uint groupIndex : SV_GroupIndex,
    out vertices MSOutput verts[3],
    out indices uint3 tris[1]
)
{
    // スレッドグループの頂点とプリミティブの数を設定.
    SetMeshOutputCounts(4, 1);

    // 頂点番号を設定 
    if (groupIndex < 2)
    {
        uint offset= groupIndex * 3;
        tris[0] = uint3(Indices[offset + 0], Indices[offset + 1], Indices[offset + 2]);
    }

    // 頂点データを設定.
    if (groupIndex < 4)
    {
        uint vertexIndex = Indices[groupIndex];
        MSInput input = Vertices[vertexIndex];
        MSOutput output = (MSOutput) 0;
        
        float4x4 wvp=mul(gVP.view, gVP.projection);
        wvp=mul(gTF.matWorld, wvp);
        float4 position = mul(input.position, wvp);

        output.position = position;
        output.texcoord = input.texcoord;
        output.normal = input.normal;
        output.Color = float4(1.0f, 1.0f, 1.0f, 1.0f);
        verts[groupIndex].position = output.position;
        verts[groupIndex].texcoord = output.texcoord;
        verts[groupIndex].normal = output.normal;
        verts[groupIndex].Color = output.Color;
    }
}
