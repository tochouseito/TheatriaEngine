#include "../header/Particle.hlsli"
#include "../header/ViewProjection.hlsli"

ConstantBuffer<ViewProjection> gPerView : register(b0);
RWStructuredBuffer<Particle> gParticles : register(u0);


struct VertexShaderInput
{
    float4 position : POSITION;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
    float4 color : COLOR0;
};

VertexShaderOutput main(VertexShaderInput input, uint instanceId : SV_InstanceID)
{
    VertexShaderOutput output;
    Particle particle = gParticles[instanceId];
    float4x4 WVP = mul(gPerView.view, gPerView.projection);
    float4x4 cameraMatrix = gPerView.matWorld;
    float cosY = cos(PI);
    float sinY = sin(PI);
    float4x4 backToFrontMatrix =
    {
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
        particle.scale.x, 0.0f, 0.0f, 0.0f,
        0.0f, particle.scale.y, 0.0f, 0.0f,
        0.0f, 0.0f, particle.scale.z, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );
    float4x4 translateMatrix = float4x4(
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        particle.translate.x, particle.translate.y, particle.translate.z, 1.0f
    );
    //float4x4 worldMatrix = MakeAffineMatrix(particle.scale, particle.rotate, particle.translate); // worldMatrixを作る
    float4x4 worldMatrix = mul(scaleMatrix, mul(billboardMatrix, translateMatrix));
    //float32_t4x4 camaraMatrix=MakeAffineMatrix(gPerView.cameraPosition)
    worldMatrix[0] *= particle.scale.x;
    worldMatrix[1] *= particle.scale.y;
    worldMatrix[2] *= particle.scale.z;
    worldMatrix[3].xyz = particle.translate;
    output.position = mul(input.position, mul(worldMatrix, WVP));
    //output.position = mul(input.position, WVP);
    output.texcoord = input.texcoord;
    //output.normal = normalize(mul(input.normal, (float3x3) gParticleWorld[instanceId].World));
    output.color = particle.color;
    return output;
}