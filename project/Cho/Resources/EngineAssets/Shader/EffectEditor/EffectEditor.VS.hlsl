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
    switch (gEffectMesh[particle.meshID].placement) {
        case PLACEMENT_TYPE_BILLBOARD:
            billboardMatrix = BillboardMatrix(cameraMatrix);
            break;
        case PLACEMENT_TYPE_BILLBOARDY:
            billboardMatrix = YAxisBillboardMatrix(cameraMatrix);
            break;
        case PLACEMENT_TYPE_BILLBOARDXY:
            //billboardMatrix = SphereBillboardMatrix(cameraMatrix, cameraPosition, particle.position.value);
            break;
        case PLACEMENT_TYPE_CONSTANT:
            //billboardMatrix = CylinderBillboardMatrix(cameraMatrix, cameraPosition, particle.position.value);
            break;
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