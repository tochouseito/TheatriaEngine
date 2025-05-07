
struct World
{
    float4x4 World;
    uint materialID;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
    uint materialID : MATERIALID0;
};