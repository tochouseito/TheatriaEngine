#include "../header/Demo.hlsli"
#include "../header/ViewProjection.hlsli"

// MSInput structure
struct MSInput
{
    float4 position : POSITION;
    float2 texcoord : TEXCOORD;
    float3 normal : NORMAL;
};

// MSOutput structure
struct MSOutput
{
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD;
    float3 normal : NORMAL;
};

// Meshlet structure
struct Meshlet
{
    uint vertexOffset; // 頂点番号オフセット
    uint vertexCount; // 出力頂点数
    uint primitiveOffset; // プリミティブ番号オフセット
    uint primitiveCount; // 出力プリミティブ数
};

// Resources
ConstantBuffer<Transform> gTransform : register(b0);
ConstantBuffer<ViewProjection> gVP : register(b1);
StructuredBuffer<MSInput> vertices : register(t0);
StructuredBuffer<uint> indices : register(t1);
StructuredBuffer<Meshlet> meshlets : register(t2);
StructuredBuffer<uint> primitives : register(t3);


//      パッキングされたインデックスデータを展開する.
uint3 UnpackPrimitiveIndex(uint packedIndex)
{
    return uint3(
        packedIndex & 0x3FF,
        (packedIndex >> 10) & 0x3FF,
        (packedIndex >> 20) & 0x3FF);
}

[numthreads(128, 1, 1)]
[outputtopology("triangle")]
void main
(
    uint groupThreadId : SV_GroupThreadID,
    uint groupId : SV_GroupID,
    out vertices MSOutput verts[64],
    out indices uint3 polys[126]
)
{
    Meshlet m = meshlets[groupId];

    SetMeshOutputCounts(m.vertexCount, m.primitiveCount);

    if (groupThreadId < m.primitiveCount)
    {
        uint packedIndex = primitives[m.primitiveOffset + groupThreadId];
        polys[groupThreadId] = UnpackPrimitiveIndex(packedIndex);
    }
    
    if (groupThreadId < m.vertexCount)
    {
        uint index = indices[m.vertexOffset + groupThreadId];
        MSInput input = vertices[index];
        MSOutput output = (MSOutput) 0;

        float4x4 wvp = mul(gVP.view, gVP.projection);
        wvp = mul(gTransform.matWorld, wvp);
        float4 position = mul(input.position, wvp);

        output.position = position;
        output.texCoord = input.texcoord;
        output.normal = input.normal;

        verts[groupThreadId] = output;
    }
}