#include "../header/Demo.hlsli"

struct PixelShaderOutput
{
    float4 pixel : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    
    output.pixel = float4(1.0f, 0.0f, 0.0f, 1.0f);

    return output;
}