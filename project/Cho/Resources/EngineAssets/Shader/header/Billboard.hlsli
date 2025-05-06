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
// Y-axis rotation billboard
float4x4 YAxisBillboardMatrix(const float4x4 cameraMatrix) {
    float3 camForward = normalize(float3(cameraMatrix._31, 0.0f, cameraMatrix._33));
    float3 camRight = normalize(float3(camForward.z, 0.0f, -camForward.x));
    float3 camUp = float3(0.0f, 1.0f, 0.0f);

    float4x4 result = {
        camRight.x, camRight.y, camRight.z, 0.0f,
        camUp.x, camUp.y, camUp.z, 0.0f,
        camForward.x, 0.0f, camForward.z, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };

    return result;
}