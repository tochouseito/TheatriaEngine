// シャドウマップに書き込むための深度バッファ
struct PixelShaderOutput {
    float4 depth : SV_TARGET0; // 深度値（Rチャネルに格納）
};

PixelShaderOutput main(float4 position : SV_POSITION) {
    PixelShaderOutput output;

    // 深度値を取得（NDC 空間で 0 ～ 1 にマップされている）
    float depthValue = position.z / position.w;

    // シャドウマップのフォーマットが R8G8B8A8_UNORM_SRGB なので、深度値をRGB成分に変換して格納
    output.depth = float4(depthValue, depthValue, depthValue, 1.0f);

    return output;
}
