struct PSInput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

struct PixelShaderOutput
{
    float4 pixel : SV_TARGET0;
};

PixelShaderOutput main(PSInput input)
{
    PixelShaderOutput output;
    
    //if (input.color.a < 1.0f)
    //{
    //    discard;
    //}
    
    output.pixel = input.color;

    return output;
}