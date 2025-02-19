
#include "../header/Demo.hlsli"

RWStructuredBuffer<Transform> gTF : register(u0);

[numthreads(1024, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    uint index = DTid.x;
    
    gTF[index].matWorld = mul(gTF[index].matWorld, gTF[index].rootNode);
}