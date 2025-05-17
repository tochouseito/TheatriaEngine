#include "../header/Demo.hlsli"
#include "../header/Material.hlsli"
#include "../header/Light.hlsli"
#include "../header/Environment.hlsli"

// ライト
ConstantBuffer<Lights> gLights : register(b1);
// 環境
ConstantBuffer<Environment> gEnvironment : register(b2);
// IntegrationTransformResource
StructuredBuffer<Transform> gITF : register(t0, space1);
// マテリアル
StructuredBuffer<Material> gIMaterial : register(t1,space1);
// テクスチャリソース(カラー)
Texture2D<float4> gTextures[] : register(t2, space0);
// サンプラー
SamplerState gSampler : register(s0);

struct PixelShaderOutput {
    float4 color : SV_TARGET0;
};

PixelShaderOutput main(VSOut input) {
    PixelShaderOutput output;
    
    // 最終出力
    float4 finalColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
    // 環境光
    float4 ambientColor = gEnvironment.ambientColor;
    // マテリアル
    Material material = gIMaterial[input.materialID];
    // テクスチャカラー
    float4 textureColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
    // 全ライト
    Light lights[MAX_LIGHTS] = gLights.lights;
    // ライティング結果
    float3 lig = float3(0.0f, 0.0f, 0.0f);
    // カメラへの方向
    float3 toEye = normalize(input.cameraPosition - input.position.xyz);
    // 法線
    float3 normal = normalize(input.normal);
    
    // テクスチャが有効ならテクスチャカラーを取得
    if (material.enableTexture != 0)
    {
        float2 texcoord = input.texcoord;
        if (material.uvFlipY)
        {
            texcoord.y = 1.0f - texcoord.y;
        }
        // テクスチャ
        float4 transformedUV = mul(float4(texcoord, 0.0f, 1.0f), gIMaterial[input.materialID].matUV);
        textureColor = gTextures[material.textureID].Sample(gSampler, transformedUV.xy);
    }
    
    // ライトのタイプごとにライティング
    for (int i = 0; i < MAX_LIGHTS; i++) {
        switch (lights[i].type) {
            case LIGHT_TYPE_DIRECTIONAL:{// 平行光源
                    // ライトが無効ならスキップ
                    if (lights[i].active == 0) {
                        continue;
                    }
                    // ライトの位置を取得
                    float3 position = {
                        gITF[lights[i].transformMapID].matWorld[3][0],
                        gITF[lights[i].transformMapID].matWorld[3][1],
                        gITF[lights[i].transformMapID].matWorld[3][2]
                    };
                
                    float3 lightDir = -normalize(lights[i].direction);
                    // 鏡面反射の強度を求める
                    float3 halfVector = normalize(lightDir + toEye);
                    float NDotH = saturate(dot(normal, halfVector)); // 鏡面反射強度
                    float specularPow = pow(NDotH, material.shininess);
                    // half lambert
                    float NdotL = dot(normal, lightDir);
                    // half lambertの計算
                    float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
                    // 拡散反射
                    float3 diffuseDirLight = lights[i].color.rgb * cos * lights[i].intensity;
                    // 鏡面反射
                    float3 specularDirLight = lights[i].color.rgb * lights[i].intensity * specularPow * float3(1.0f, 1.0f, 1.0f);
                    // 結果を加算
                    lig += diffuseDirLight + specularDirLight;
                    break;
                }
            case LIGHT_TYPE_POINT:{// 点光源
                    // ライトが無効ならスキップ
                    if (lights[i].active == 0) {
                        continue;
                    }
                    // ライトの位置を取得
                    float3 position = {
                        gITF[lights[i].transformMapID].matWorld[3][0],
                        gITF[lights[i].transformMapID].matWorld[3][1],
                        gITF[lights[i].transformMapID].matWorld[3][2]
                    };
                    break;
                }
            case LIGHT_TYPE_SPOT:{// スポットライト
                    // ライトが無効ならスキップ
                    if (lights[i].active == 0) {
                        continue;
                    }
                    // ライトの位置を取得
                    float3 position = {
                        gITF[lights[i].transformMapID].matWorld[3][0],
                        gITF[lights[i].transformMapID].matWorld[3][1],
                        gITF[lights[i].transformMapID].matWorld[3][2]
                    };
                    break;
                }
        }
    }
    // ライティング結果と環境光を加算
    lig.rgb += ambientColor.rgb;
    // 合計
    finalColor.rgb = material.color.rgb * textureColor.rgb * lig.rgb;
    finalColor.a = material.color.a * textureColor.a;
    // 最終出力
    output.color = finalColor;

    return output;
}