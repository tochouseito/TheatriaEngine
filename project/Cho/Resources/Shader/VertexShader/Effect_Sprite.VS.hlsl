#include "../header/ViewProjection.hlsli"
#include "../header/Effect.hlsli"
#include "../header/Particle.hlsli"

// CBV: ビュープロジェクションリソース
ConstantBuffer<ViewProjection> gVP : register(b0);

// SRV: エフェクトノード
StructuredBuffer<EffectNode> gEffectNode : register(t0);

// UAV: パーティクルリソース
RWStructuredBuffer<EffectParticle> gParticle : register(u0);

//// UAV: 頂点バッファ
//RWStructuredBuffer<VertexData> vertices : register(u0);

//// UAV: インデックスバッファ
//RWStructuredBuffer<uint> indices : register(u1);

// Y 軸固定のビルボード行列を計算する関数
float4x4 CalculateYAxisBillboardMatrix(float3 particlePos, float3 cameraPos)
{
    // カメラからパーティクルへの方向ベクトル
    float3 forward = normalize(cameraPos - particlePos);
    forward.y = 0; // Y 軸方向の回転を防ぐ
    forward = normalize(forward); // 正規化

    // Y 軸固定の up ベクトル
    float3 up = float3(0, 1, 0);

    // X 軸の right ベクトルを計算 (右手座標系)
    float3 right = normalize(cross(up, forward));

    // Z 軸の forward ベクトルを再計算（直交を保証）
    forward = cross(right, up);

    // 行列を構築
    float4x4 billboardMatrix = float4x4(
        right.x, right.y, right.z, 0.0f,
        up.x, up.y, up.z, 0.0f,
        forward.x, forward.y, forward.z, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );

    return billboardMatrix;
}

float4x4 RotateX(float angle)
{
    float s = sin(angle);
    float c = cos(angle);

    return float4x4(
        1, 0, 0, 0,
        0, c, s, 0,
        0, -s, c, 0,
        0, 0, 0, 1
    );
}

float4x4 RotateY(float angle)
{
    float s = sin(angle);
    float c = cos(angle);

    return float4x4(
         c, 0, -s, 0,
         0, 1, 0, 0,
         s, 0, c, 0,
         0, 0, 0, 1
    );
}

float4x4 RotateZ(float angle)
{
    float s = sin(angle);
    float c = cos(angle);

    return float4x4(
        c, s, 0, 0,
       -s, c, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    );
}

float4x4 RotateXYZ(float3 angles)
{
    return mul(RotateZ(angles.z), mul(RotateY(angles.y), RotateX(angles.x)));
}



struct VertexShaderInput
{
    float4 position : POSITION;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
    float4 color : COLOR0;
};

VertexShaderOutput main(VertexShaderInput input , uint instanceId : SV_InstanceID)
{
    VertexShaderOutput output;
    
    // パーティクル
    EffectParticle particle = gParticle[instanceId];
    
    // worldViewProjection
    float4x4 WVP = mul(gVP.view, gVP.projection);
    
    // カメラ行列
    float4x4 cameraMatrix = gVP.matWorld;
    
    float3 cameraPos = gVP.cameraPosition;
    
    // scaleMatrix
    float4x4 scaleMatrix = float4x4(
        particle.scale.value.x, 0.0f, 0.0f, 0.0f,
        0.0f, particle.scale.value.y, 0.0f, 0.0f,
        0.0f, 0.0f, particle.scale.value.z, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );
    
    // translateMatrix
    float4x4 translateMatrix = float4x4(
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        particle.position.value.x, particle.position.value.y, particle.position.value.z, 1.0f
    );
    
    float4x4 worldMatrix;
    
    float4x4 billboardMatrix;
    if (gEffectNode[0].draw.meshSprite.placement == 2)
    {
        float4x4 localRotationMatrix = RotateXYZ(particle.rotation.value);
        // Y 軸固定のビルボード行列を計算
        billboardMatrix = CalculateYAxisBillboardMatrix(particle.position.value, cameraPos);
        
        // worldMatrix
        worldMatrix = mul(translateMatrix, mul(billboardMatrix, mul(scaleMatrix, localRotationMatrix)));
        worldMatrix[0] *= particle.scale.value.x;
        worldMatrix[1] *= particle.scale.value.y;
        worldMatrix[2] *= particle.scale.value.z;
        worldMatrix[3].xyz = particle.position.value;
    }
    else if (gEffectNode[0].draw.meshSprite.placement == 0)
    {
        // billboardMatrix
        float cosY = cos(PI);
        float sinY = sin(PI);
        float4x4 backToFrontMatrix =
        {
            cosY, 0.0f, -sinY, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        sinY, 0.0f, cosY, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
        };
    
        billboardMatrix = mul(backToFrontMatrix, cameraMatrix);
        billboardMatrix[3][0] = 0.0f;
        billboardMatrix[3][1] = 0.0f;
        billboardMatrix[3][2] = 0.0f;
        
        // worldMatrix
        worldMatrix = mul(translateMatrix, mul(billboardMatrix, scaleMatrix));
        worldMatrix[0] *= particle.scale.value.x;
        worldMatrix[1] *= particle.scale.value.y;
        worldMatrix[2] *= particle.scale.value.z;
        worldMatrix[3].xyz = particle.position.value;
    }
    else
    {
        billboardMatrix = RotateXYZ(particle.rotation.value);
        // worldMatrix
        worldMatrix = mul(translateMatrix, mul(billboardMatrix, scaleMatrix));
        worldMatrix[0] *= particle.scale.value.x;
        worldMatrix[1] *= particle.scale.value.y;
        worldMatrix[2] *= particle.scale.value.z;
        worldMatrix[3].xyz = particle.position.value;
    }
    
    // 頂点座標
    output.position = mul(input.position, mul(worldMatrix, WVP));
    
    // テクスチャ座標
    output.texcoord = input.texcoord;
    
    // 法線
    //output.normal = input.normal;
    
    // カラー
    output.color = particle.color;

    return output;
}