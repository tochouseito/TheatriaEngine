#include "../header/Sprite.hlsli"

ConstantBuffer<World> gWorld : register(b0);

struct SpriteVertexShaderInput
{
    float4 position : POSITION;
    float2 texcoord : TEXCOORD0;
};
SpriteVertexShaderOutput main(SpriteVertexShaderInput input)
{
    SpriteVertexShaderOutput output;
    output.position = mul(input.position, gWorld.World);
    output.texcoord = input.texcoord;
    
    return output;
}