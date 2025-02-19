#include "../header/Atmosphere.hlsli"

// ピクセルシェーダー
float4 main(float4 position : SV_Position, float2 texcoord : TEXCOORD) : SV_Target
{
    // カメラベクトルの計算
    float3 camera_vector = get_camera_vector(iResolution, texcoord);

    // カメラ位置の設定
    float3 camera_position = PLANET_POS + float3(0.0, ATMOS_RADIUS, 0.0);

    // ライト方向
    float3 light_dir = normalize(float3(0.0, cos(iTime / 8.0), sin(iTime / 8.0)));

    // シーンのレンダリング（ここでは緑の球体）
    float3 scene_color = float3(0.0, 0.25, 0.05);
    float max_dist = 1e12; // 最大距離（適切に設定する）

    // 大気の色を計算
    float3 col = calculate_scattering(
        camera_position, camera_vector, max_dist, scene_color, light_dir, float3(40.0, 40.0, 40.0),
        PLANET_POS, PLANET_RADIUS, ATMOS_RADIUS, RAY_BETA, MIE_BETA, ABSORPTION_BETA, AMBIENT_BETA,
        G, HEIGHT_RAY, HEIGHT_MIE, HEIGHT_ABSORPTION, ABSORPTION_FALLOFF, PRIMARY_STEPS, LIGHT_STEPS
    );

    // トーンマッピング
    col = 1.0 - exp(-col);

    // 結果を返す
    return float4(col, 1.0);
}
