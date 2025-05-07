#include "../header/UIStandard.hlsli"

StructuredBuffer<World> gWorlds : register(t0);
StructuredBuffer<uint> gUIids : register(t1);

struct VSInput
{
    float4 position : POSITION;
    float2 texcoord : TEXCOORD0;
    float4 color : COLOR;
};
VSOutput main(VSInput input,uint instanceID : SV_InstanceID)
{
    VSOutput output;
    output.position = mul(input.position, gWorlds[gUIids[instanceID]].World);
    output.texcoord = input.texcoord;
    output.materialID = gWorlds[gUIids[instanceID]].materialID;
    
    return output;
}