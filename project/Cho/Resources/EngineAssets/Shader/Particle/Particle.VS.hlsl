#include "../header/ViewProjection.hlsli"
#include "../header/Particle.hlsli"

// CBV: ビュープロジェクションリソース
ConstantBuffer<ViewProjection> gVP : register(b0);

// UAV: パーティクルリソース
RWStructuredBuffer<Particle> gParticles : register(u0);

struct VSInput {
    float4 pos : POSITION;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL;
    float4 color : COLOR;
    uint vertexID : VERTEXID;
};

VSOutput main(VSInput input, uint instanceId : SV_InstanceID) {
    VSOutput output;
    Particle particle = gParticles[instanceId];
    float4x4 WVP = mul(gVP.view, gVP.projection);
    float4x4 cameraMatrix = gVP.matWorld;
    float cosY = cos(PI);
    float sinY = sin(PI);
    float4x4 backToFrontMatrix = {
        cosY, 0.0f, -sinY, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		sinY, 0.0f, cosY, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
    };
    float4x4 billboardMatrix = mul(backToFrontMatrix, cameraMatrix);
    billboardMatrix[3][0] = 0.0f;
    billboardMatrix[3][1] = 0.0f;
    billboardMatrix[3][2] = 0.0f;
    float4x4 scaleMatrix = float4x4(
        particle.scale.value.x, 0.0f, 0.0f, 0.0f,
        0.0f, particle.scale.value.y, 0.0f, 0.0f,
        0.0f, 0.0f, particle.scale.value.z, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );
    float4x4 translateMatrix = float4x4(
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        particle.position.value.x, particle.position.value.y, particle.position.value.z, 1.0f
    );
    //float4x4 worldMatrix = MakeAffineMatrix(particle.scale, particle.rotate, particle.translate); // worldMatrixを作る
    float4x4 worldMatrix = mul(scaleMatrix, mul(billboardMatrix, translateMatrix));
    //float32_t4x4 camaraMatrix=MakeAffineMatrix(gPerView.cameraPosition)
    worldMatrix[0] *= particle.scale.value.x;
    worldMatrix[1] *= particle.scale.value.y;
    worldMatrix[2] *= particle.scale.value.z;
    worldMatrix[3].xyz = particle.position.value.xyz;
    output.position = mul(input.pos, mul(worldMatrix, WVP));
    //output.position = mul(input.position, WVP);
    output.texcoord = input.uv;
    //output.normal = normalize(mul(input.normal, (float3x3) gParticleWorld[instanceId].World));
    output.color = particle.color;
    return output;
}