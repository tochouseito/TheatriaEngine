#include "../header/ViewProjection.hlsli"
#include "../header/Light.hlsli"

ConstantBuffer<ViewProjection> gVP : register(b0);

ConstantBuffer<PunctualLights> gLight : register(b1);

Texture2D<float4> albedoTexture : register(t0); // アルベド
Texture2D<float4> normalTexture : register(t1); // 法線
Texture2D<float4> positionTexture : register(t2); // 位置
Texture2D<float4> materialTexture : register(t3); // マテリアル情報
Texture2D<float> depthTexture : register(t4); // 深度

SamplerState gSampler : register(s0);

struct VertexShaderOutput {
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
};

struct PixelShaderOutput {
    float4 pixel : SV_TARGET0;
};

// メインピクセルシェーダー
PixelShaderOutput main(VertexShaderOutput input) {
    PixelShaderOutput output;
    
    // Gバッファから情報を取得
    float4 albedo = albedoTexture.Sample(gSampler, input.texcoord);
    float3 normal = normalTexture.Sample(gSampler, input.texcoord).xyz;
    float3 position = positionTexture.Sample(gSampler, input.texcoord).xyz;
    float4 material = materialTexture.Sample(gSampler, input.texcoord);
    float depth = depthTexture.Sample(gSampler, input.texcoord);
    
    if (depth == 1.0f) {
        output.pixel = float4(0.0f, 0.0f, 0.0f, 1.0f);
        return output;
    }
    
    normal = normalize((normal * 2.0f) - 1.0f);
    float shininess = material.z;
    int enableLighting = material.w != 0 ? 1 : 0;
    
    if (enableLighting == 0) {
        output.pixel = albedo;
        output.pixel.a = 1.0;
        return output;
    }
    
    // カメラへの方向を算出
    float3 toEye = normalize(gVP.cameraPosition - position);
    
    float3 ambient = gLight.ambientLight;
    
    float4 finalColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
    
    float3 lig = float3(0.0f, 0.0f, 0.0f);
    
    // Directional Light
    for (int dirLIndex = 0; dirLIndex < MAX_DIRECTIONAL_LIGHTS; dirLIndex++) {
        
        // ディレクショナルライトがアクティブでない場合はスキップ
        if (!gLight.directionalLights[dirLIndex].active) {
            continue;
        }
        
        float3 lightDir = -normalize(gLight.directionalLights[dirLIndex].direction);
        
        // 鏡面反射の強度を求める
        float3 halfVector = normalize(lightDir + toEye);
        float NDotH = saturate(dot(normal, halfVector)); // 鏡面反射強度
        float specularPow = pow(NDotH, shininess);
        
        // 階調ライティング
        float NdotL = saturate(dot(normal, lightDir)); // ライトと法線の内積
        float toonStep = 0.22; // 階調の幅
        float toonShade = floor(NdotL / toonStep) * toonStep; // 階調化
        
        // 拡散反射
        float3 diffuseDirectionalLight =
        gLight.directionalLights[dirLIndex].color.rgb * gLight.directionalLights[dirLIndex].intensity * toonShade;
        
        // 鏡面反射
        float3 specularDirectionalLight = 
        gLight.directionalLights[dirLIndex].color.rgb *
        gLight.directionalLights[dirLIndex].intensity *
        specularPow * 
        float3(1.0f, 1.0f, 1.0f);
        
        // 結果を加算
        lig += diffuseDirectionalLight + specularDirectionalLight;
    }
    
    // Point Light
    for (int pointLIndex = 0; pointLIndex < MAX_POINT_LIGHTS; pointLIndex++) {
        // ポイントライトがアクティブでない場合はスキップ
        if (!gLight.pointLights[pointLIndex].active) {
            continue;
        }
        
        // 物体の特定の点に対する入射光を計算する
        float3 pointLightDirection = gLight.pointLights[pointLIndex].position - position;
        float distance = length(pointLightDirection); // ポイントライトへの距離
        
        // 範囲外であればスキップ
        if (distance > gLight.pointLights[pointLIndex].radius) {
            continue;
        }
        
        pointLightDirection = normalize(pointLightDirection);
        
        // 減衰係数の計算
        // 逆二乗測による係数,指数によるコントロール
        float factor = pow(saturate(-distance / gLight.pointLights[pointLIndex].radius + 1.0), gLight.pointLights[pointLIndex].decay);
        float attenuation = 1.0f / (1.0f + gLight.pointLights[pointLIndex].decay * (distance / gLight.pointLights[pointLIndex].radius));
        
        // 階調ライティング
        float NdotL = saturate(dot(normal, pointLightDirection)); // ライトと法線の内積
        float toonStep = 0.22; // 階調の幅
        float toonShade = floor(NdotL / toonStep) * toonStep; // 階調化
        
        // 拡散反射
        float3 diffusePointLight = gLight.pointLights[pointLIndex].color.rgb * gLight.pointLights[pointLIndex].intensity * factor * toonShade;
        
        // 鏡面反射の強度を求める
        float3 halfVectorPointLight = normalize(pointLightDirection + toEye);
        float NDotHPointLight = saturate(dot(normal, halfVectorPointLight)); // 鏡面反射強度
        float specularPowPointLight = pow(NDotHPointLight, shininess);
        
        // 鏡面反射
        float3 specularPointLight = gLight.pointLights[pointLIndex].color.rgb * specularPowPointLight * gLight.pointLights[pointLIndex].intensity * attenuation * factor;
        
        // 結果を加算
        lig += diffusePointLight + specularPointLight;
    }
    
    // Spot Light
    for (int spotLIndex = 0; spotLIndex < MAX_SPOT_LIGHTS; spotLIndex++) {
        // スポットライトがアクティブでない場合はスキップ
        if (!gLight.spotLights[spotLIndex].active) {
            continue;
        }
        
        // 物体の特定の点に対する入射光を計算する
        float3 spotLightDirectionOnSurface = position - gLight.spotLights[spotLIndex].position;
        float distanceSpotLight = length(spotLightDirectionOnSurface); // ポイントライトへの距離
        spotLightDirectionOnSurface = normalize(spotLightDirectionOnSurface);
        
        // 範囲外であればスキップ
        if (distanceSpotLight > gLight.spotLights[spotLIndex].distance) {
            continue;
        }
        
        float cosAngle = dot(spotLightDirectionOnSurface, normalize(gLight.spotLights[spotLIndex].direction));
        
        //// 角度がコーンの外側であればスキップ
        //if (cosAngle < gLight.spotLights[spotLIndex].cosAngle) {
        //    continue;
        //}
        
        float falloffFactor;
        if (gLight.spotLights[spotLIndex].cosAngle == gLight.spotLights[spotLIndex].cosFalloffStart)
        {
            falloffFactor = saturate((cosAngle - gLight.spotLights[spotLIndex].cosAngle) / (1.0f - gLight.spotLights[spotLIndex].cosAngle));
        }
        else
        {
            falloffFactor = saturate((cosAngle - gLight.spotLights[spotLIndex].cosAngle) / (gLight.spotLights[spotLIndex].cosFalloffStart - gLight.spotLights[spotLIndex].cosAngle));
        }
        
        // 減衰係数の計算
        // 逆二乗測による係数,指数によるコントロール
        float factorSpotLight = pow(saturate(-distanceSpotLight / gLight.spotLights[spotLIndex].distance + 1.0), gLight.spotLights[spotLIndex].decay);
        //float attenuation = 1.0f / (1.0f + gLight.spotLights[spotLIndex].decay * (distanceSpotLight / gLight.spotLights[spotLIndex].distance));
        
        //float falloffFactor = saturate((cosAngle - gLight.spotLights[spotLIndex].cosAngle) / (gLight.spotLights[spotLIndex].cosFalloffStart - gLight.spotLights[spotLIndex].cosAngle));
         
        float finalAttenuation = //attenuation * 
        factorSpotLight * falloffFactor;
        
        // 階調ライティング
        float NdotL = saturate(dot(normal, spotLightDirectionOnSurface)); // ライトと法線の内積
        float toonStep = 0.22; // 階調の幅
        float toonShade = floor(NdotL / toonStep) * toonStep; // 階調化
        
        // 拡散反射
        float3 diffuseSpotLight = gLight.spotLights[spotLIndex].color.rgb * gLight.spotLights[spotLIndex].intensity * finalAttenuation * toonShade;
        
        // 鏡面反射の強度を求める
        float3 halfVectorSpotLight = normalize(-spotLightDirectionOnSurface + toEye);
        float NDotHSpotLight = saturate(dot(normal, halfVectorSpotLight)); // 鏡面反射強度
        float specularPowSpotLight = pow(NDotHSpotLight, shininess);
        
        // 鏡面反射
        float3 specularSpotLight = gLight.spotLights[spotLIndex].color.rgb * specularPowSpotLight * gLight.spotLights[spotLIndex].intensity * finalAttenuation;
        
        // 結果を加算
        lig += diffuseSpotLight + specularSpotLight;
    }
    
    lig += ambient;
    
    finalColor = albedo;
    
    finalColor.rgb *= lig;
    
    output.pixel = finalColor;
    
    return output;
}
