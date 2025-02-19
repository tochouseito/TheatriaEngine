// 定数バッファ
cbuffer Constants : register(b0)
{
    float3 iResolution; // 画面解像度
    float4 iMouse; // マウス位置 (x, y, z, w)
    float iTime; // 経過時間
}

// 定数の定義
#define PLANET_POS float3(0.0, 0.0, 0.0)
#define PLANET_RADIUS 6371000.0
#define ATMOS_RADIUS 6471000.0

#define RAY_BETA float3(5.5e-6, 13.0e-6, 22.4e-6)
#define MIE_BETA float3(21e-6, 21e-6, 21e-6)
#define ABSORPTION_BETA float3(2.04e-5, 4.97e-5, 1.95e-6)
#define AMBIENT_BETA float3(0.0, 0.0, 0.0)

#define G 0.7
#define HEIGHT_RAY 8000.0
#define HEIGHT_MIE 1200.0
#define HEIGHT_ABSORPTION 30000.0
#define ABSORPTION_FALLOFF 4000.0

#define PRIMARY_STEPS 32
#define LIGHT_STEPS 8

float3 calculate_scattering(
    float3 start, float3 dir, float max_dist, float3 scene_color, float3 light_dir, float3 light_intensity,
    float3 planet_position, float planet_radius, float atmo_radius, float3 beta_ray, float3 beta_mie,
    float3 beta_absorption, float3 beta_ambient, float g, float height_ray, float height_mie,
    float height_absorption, float absorption_falloff, int steps_i, int steps_l
)
{
    // オフセットを適用してカメラ位置を調整
    start -= planet_position;

    // レイの始点と終点を計算
    float a = dot(dir, dir);
    float b = 2.0 * dot(dir, start);
    float c = dot(start, start) - (atmo_radius * atmo_radius);
    float d = (b * b) - 4.0 * a * c;

    // 交差がない場合、シーンの色を返す
    if (d < 0.0)
    {
        return scene_color;
    }

    // レイの長さを計算
    float2 ray_length = float2(
        max((-b - sqrt(d)) / (2.0 * a), 0.0),
        min((-b + sqrt(d)) / (2.0 * a), max_dist)
    );

    // レイが大気にヒットしない場合、シーンの色を返す
    if (ray_length.x > ray_length.y)
    {
        return scene_color;
    }

    // レイ長さのクランプ
    ray_length.y = min(ray_length.y, max_dist);
    ray_length.x = max(ray_length.x, 0.0);

    // レイのステップサイズ
    float step_size_i = (ray_length.y - ray_length.x) / steps_i;
    float ray_pos_i = ray_length.x + step_size_i * 0.5;

    // 結果を格納する変数
    float3 total_ray = float3(0.0, 0.0, 0.0);
    float3 total_mie = float3(0.0, 0.0, 0.0);

    // 光学的な深さ
    float3 opt_i = float3(0.0, 0.0, 0.0);

    // スケール高さを計算
    float2 scale_height = float2(height_ray, height_mie);

    // フェーズ関数を計算
    float mu = dot(dir, light_dir);
    float mumu = mu * mu;
    float gg = g * g;
    float phase_ray = (3.0 / (16.0 * 3.14159265359)) * (1.0 + mumu);
    float phase_mie = (3.0 / (8.0 * 3.14159265359)) * ((1.0 - gg) * (mumu + 1.0)) / pow(1.0 + gg - 2.0 * mu * g, 1.5);

    // 主レイのサンプリング
    for (int i = 0; i < steps_i; ++i)
    {
        // サンプル位置を計算
        float3 pos_i = start + dir * ray_pos_i;

        // 高さを計算
        float height_i = length(pos_i) - planet_radius;

        // 密度を計算
        float3 density = float3(exp(-height_i / scale_height.x), exp(-height_i / scale_height.y), 0.0);

        // 吸収密度を計算
        float denom = (height_absorption - height_i) / absorption_falloff;
        density.z = (1.0 / (denom * denom + 1.0)) * density.x;

        // 密度にステップサイズを掛ける
        density *= step_size_i;

        // 光学的な深さに追加
        opt_i += density;

        // 光レイのステップを計算
        float step_size_l = step_size_i / steps_l;
        float ray_pos_l = step_size_l * 0.5;
        float3 opt_l = float3(0.0, 0.0, 0.0);

        // 光レイのサンプリング
        for (int l = 0; l < steps_l; ++l)
        {
            float3 pos_l = pos_i + light_dir * ray_pos_l;
            float height_l = length(pos_l) - planet_radius;
            float3 density_l = float3(exp(-height_l / scale_height.x), exp(-height_l / scale_height.y), 0.0);
            density_l.z = (1.0 / (denom * denom + 1.0)) * density_l.x;
            density_l *= step_size_l;
            opt_l += density_l;
            ray_pos_l += step_size_l;
        }

        // 減衰を計算
        float3 attn = exp(-beta_ray * (opt_i.x + opt_l.x) - beta_mie * (opt_i.y + opt_l.y) - beta_absorption * (opt_i.z + opt_l.z));

        // 累積散乱を計算
        total_ray += density.x * attn;
        total_mie += density.y * attn;

        // 次のサンプル位置に進む
        ray_pos_i += step_size_i;
    }

    // 光学的減衰
    float3 opacity = exp(-(beta_mie * opt_i.y + beta_ray * opt_i.x + beta_absorption * opt_i.z));

    // 最終的な色を計算して返す
    return (
        phase_ray * beta_ray * total_ray +
        phase_mie * beta_mie * total_mie +
        opt_i.x * beta_ambient
    ) * light_intensity + scene_color * opacity;
}


// カメラベクトルの取得
float3 get_camera_vector(float3 resolution, float2 coord)
{
    float2 uv = coord / resolution.xy - float2(0.5, 0.5);
    uv.x *= resolution.x / resolution.y;
    return normalize(float3(uv, -1.0));
}