
float4x4 CalculateYAxisBillboardMatrix(float3 particlePos, float3 cameraPos)
{
    
    float3 forward = normalize(cameraPos - particlePos);
    forward.y = 0; 
    forward = normalize(forward); 

    
    float3 up = float3(0, 1, 0);

   
    float3 right = normalize(cross(up, forward));

   
    forward = cross(right, up);

   
    float4x4 billboardMatrix = float4x4(
        right.x, right.y, right.z, 0.0f,
        up.x, up.y, up.z, 0.0f,
        forward.x, forward.y, forward.z, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );

    return billboardMatrix;
}
