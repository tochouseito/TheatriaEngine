struct Transform
{
    float4x4 matWorld;
    float4x4 worldInverse;
    float4x4 rootNode;
};

struct VertexShaderOutput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
    float3 worldPosition : POSITION0;
    float3 cameraPosition : CAMERAPOS0;
};

struct VSOut 
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
    float3 worldPosition : POSITION0;
    float3 cameraPosition : CAMERAPOS0;
};

struct ViewProjection
{
    float4x4 view;
    float4x4 projection;
    float4x4 projectionInverse;
    float4x4 matWorld;
    float4x4 matBillboard;
    float3 cameraPosition;
};

struct Material
{
    float4 color;
    int enableLighting;
    float4x4 matUV;
    float shininess;
};