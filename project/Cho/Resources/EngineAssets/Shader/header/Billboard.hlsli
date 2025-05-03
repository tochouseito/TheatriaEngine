#include "Math.hlsli"

// ALLBillboard
float4x4 BillboardMatrix(const float4x4 cameraMatrix) {
    float4x4 result;
    
    float cosY = cos(PI);
    float sinY = sin(PI);
    
    float4x4 backToFrontMatrix = {
        cosY, 0.0f, -sinY, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        sinY, 0.0f, cosY, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f};
    
    result = mul(backToFrontMatrix, cameraMatrix);
    result[3][0] = 0.0f;
    result[3][1] = 0.0f;
    result[3][2] = 0.0f;
    
    return result;
}