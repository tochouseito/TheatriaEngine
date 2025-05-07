#include "../header/Effect.hlsli"
#include "../header/Billboard.hlsli"
#include "../header/ViewProjection.hlsli"

// CBV: ビュープロジェクションリソース
ConstantBuffer<ViewProjection> gVP : register(b0);
// CBV : EffectRoot
ConstantBuffer<EffectRoot> gEffectRoot : register(b1);
// SRV : EffectNode
StructuredBuffer<EffectNode> gEffectNode : register(t0);
// SRV : EffectMeshData
StructuredBuffer<EffectSprite> gEffectMesh : register(t1);

// UAV : EffectParticle 128x1024個
RWStructuredBuffer<EffectParticle> gEffectParticle : register(u0);

float4x4 FixedYAxisBillboardMatrix(float3 particleRotation, float4x4 cameraMatrix)
{
    // Extract the camera's forward direction (Z-axis), ignore Y to keep it horizontal
    float3 camForward = normalize(float3(cameraMatrix._31, 0.0f, cameraMatrix._33));

    // Compute the camera's right vector using cross product with world up
    float3 camRight = normalize(cross(float3(0, 1, 0), camForward));

    // World up vector remains fixed on the Y axis
    float3 up = float3(0, 1, 0);

    // Recalculate forward vector to ensure orthogonality
    float3 forward = normalize(cross(camRight, up));

    // Build a Y-axis rotation matrix using particle's Y rotation
    float cosY = cos(particleRotation.y);
    float sinY = sin(particleRotation.y);

    float3x3 yRotMatrix =
    {
        cosY, 0, -sinY,
        0, 1, 0,
        sinY, 0, cosY
    };

    // Construct the billboard orientation matrix from camera-based axes
    float3x3 billboardAxis =
    {
        camRight,
        up,
        forward
    };

    // Apply particle Y-rotation to the billboard orientation
    float3x3 worldRot = mul(yRotMatrix, billboardAxis);

    // Convert 3x3 rotation matrix into a full 4x4 matrix
    float4x4 result = float4x4(
        float4(worldRot[0], 0),
        float4(worldRot[1], 0),
        float4(worldRot[2], 0),
        float4(0, 0, 0, 1)
    );

    return result;
}



VSOutput main(VSInput input, uint instanceId : SV_InstanceID,uint vertexId : SV_VertexID){
    VSOutput output;
    
    EffectParticle particle = gEffectParticle[instanceId];
    
    // worldViewProjection
    float4x4 WVP = mul(gVP.view, gVP.projection);
    // cameraMatrix
    float4x4 cameraMatrix = gVP.matWorld;
    // cameraPosition
    float3 cameraPosition = gVP.cameraPosition;
    // scaleMatrix
    float4x4 scaleMatrix = ScaleMatrix(particle.scale.value);
    // translateMatrix
    float4x4 translateMatrix = TranslateMatrix(particle.position.value);
    // rotationMatrix
    
    // billboardMatrix
    float4x4 billboardMatrix;
    switch (gEffectMesh[particle.meshID].placement)
    {
        case PLACEMENT_TYPE_BILLBOARD:{
                billboardMatrix = BillboardMatrix(cameraMatrix);
                break;
            }
        case PLACEMENT_TYPE_BILLBOARDY:{
                float3 radian = DegreesToRadians(particle.rotation.value);
                billboardMatrix = FixedYAxisBillboardMatrix(radian, cameraMatrix);
                break;
            }
        case PLACEMENT_TYPE_BILLBOARDXY:{
                float3 radian = DegreesToRadians(particle.rotation.value);
                //billboardMatrix = SphereBillboardMatrix(cameraMatrix, cameraPosition, particle.position.value);
                break;
            }
        case PLACEMENT_TYPE_CONSTANT:{
                float3 radian = DegreesToRadians(particle.rotation.value);
                billboardMatrix = RotateXYZ(radian);
                break;
            }
    }
    // worldMatrix
    float4x4 worldMatrix = mul(mul(scaleMatrix, billboardMatrix), translateMatrix);
    worldMatrix[0] *= particle.scale.value.x;
    worldMatrix[1] *= particle.scale.value.y;
    worldMatrix[2] *= particle.scale.value.z;
    worldMatrix[3].xyz = particle.position.value;
    
    // 頂点座標タイプで頂点座標を変更
    float4 vertexPosition = input.position;
    
    if (gEffectMesh[particle.meshID].VertexPositionType == VERTEX_TYPE_CONSTANT) {
        switch (input.vertexID) {
            case 0:
                vertexPosition.xy = gEffectMesh[particle.meshID].vertexPosition.rightTop;
                break;
            case 1:
                vertexPosition.xy = gEffectMesh[particle.meshID].vertexPosition.leftTop;
                break;
            case 2:
                vertexPosition.xy = gEffectMesh[particle.meshID].vertexPosition.rightBottom;
                break;
            case 3:
                vertexPosition.xy = gEffectMesh[particle.meshID].vertexPosition.leftBottom;
                break;
            case 4:
                vertexPosition.xy = gEffectMesh[particle.meshID].vertexPosition.leftBottom;
                break;
            case 5:
                vertexPosition.xy = gEffectMesh[particle.meshID].vertexPosition.rightBottom;
                break;
        }
    }
    
    output.position = mul(vertexPosition, mul(worldMatrix, WVP));
    output.texcoord = input.texcoord;
    output.normal = input.normal;
    output.color = particle.color;
    output.instanceId = instanceId;
    
    return output;
}