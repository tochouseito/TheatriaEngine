// LineDrawingVertexShaderInput
struct VSInput {
    float3 position : POSITIONT;
    float4 color : COLOR;
};
// LineDrawingPixelShaderInput
struct PSInput {
    float4 position : SV_POSITION;
    float4 color : COLOR;
};