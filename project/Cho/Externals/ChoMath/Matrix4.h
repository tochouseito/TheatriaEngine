#pragma once
#include <cmath>
#include <cstring>

/// <summary>
/// 4x4行列構造体
/// </summary>
struct Matrix4 final {
    float m[4][4] = {};

    // 単位行列として初期化
    void InitializeIdentity() {
        std::memset(m, 0, sizeof(m));
        for (int i = 0; i < 4; ++i) {
            m[i][i] = 1.0f;
        }
    }

    // 等号演算子
    bool operator==(const Matrix4& other) const {
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                if (std::fabs(m[i][j] - other.m[i][j]) > 1e-6f) {
                    return false; // 誤差が許容範囲を超えた場合、等しくない
                }
            }
        }
        return true; // 全要素が等しければtrue
    }

    // 否定の等号演算子
    bool operator!=(const Matrix4& other) const {
        return !(*this == other);
    }

    // 行列加算
    Matrix4 operator+(const Matrix4& other) const {
        Matrix4 result;
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                result.m[i][j] = m[i][j] + other.m[i][j];
            }
        }
        return result;
    }

    // 行列減算
    Matrix4 operator-(const Matrix4& other) const {
        Matrix4 result;
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                result.m[i][j] = m[i][j] - other.m[i][j];
            }
        }
        return result;
    }

    // スカラー乗算
    Matrix4 operator*(float scalar) const {
        Matrix4 result;
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                result.m[i][j] = m[i][j] * scalar;
            }
        }
        return result;
    }

    // 行列積
    Matrix4 Multiply(const Matrix4& other) const {
        Matrix4 result;
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                result.m[i][j] = 0.0f;
                for (int k = 0; k < 4; ++k) {
                    result.m[i][j] += m[i][k] * other.m[k][j];
                }
            }
        }
        return result;
    }

    // 行列積（演算子）
    Matrix4 operator*(const Matrix4& other) const {
        return Multiply(other);
    }

    // 転置行列
    void Transpose(){
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                m[i][j] = m[j][i];
            }
        }
    }

    void Inverse() {
        const int MatNum = 4; // 行列の次元
        float sweep[MatNum][MatNum * 2] = {};

        // 初期化：sweepの左側に行列m、右側に単位行列をセット
        for (int i = 0; i < MatNum; i++) {
            for (int j = 0; j < MatNum; j++) {
                sweep[i][j] = m[i][j];
                sweep[i][j + MatNum] = (i == j) ? 1.0f : 0.0f;
            }
        }

        // ガウス・ジョルダン法による逆行列計算
        for (int k = 0; k < MatNum; k++) {
            // ピボット選択
            float max = fabs(sweep[k][k]);
            int max_i = k;
            for (int i = k + 1; i < MatNum; i++) {
                if (fabs(sweep[i][k]) > max) {
                    max = fabs(sweep[i][k]);
                    max_i = i;
                }
            }

            // ピボットが小さすぎる場合はエラー（非正則な行列）
            if (fabs(max) < 1e-6) {
                InitializeIdentity(); // 逆行列が存在しない場合、単位行列を返す
                return;
            }

            // 行の入れ替え
            if (k != max_i) {
                for (int j = 0; j < MatNum * 2; j++) {
                    std::swap(sweep[k][j], sweep[max_i][j]);
                }
            }

            // ピボット行を1にスケール
            float pivot = sweep[k][k];
            for (int j = 0; j < MatNum * 2; j++) {
                sweep[k][j] /= pivot;
            }

            // ピボット列の他の要素をゼロにする
            for (int i = 0; i < MatNum; i++) {
                if (i != k) {
                    float factor = sweep[i][k];
                    for (int j = 0; j < MatNum * 2; j++) {
                        sweep[i][j] -= sweep[k][j] * factor;
                    }
                }
            }
        }

        // 結果をMatrix4にコピー
        for (int i = 0; i < MatNum; i++) {
            for (int j = 0; j < MatNum; j++) {
                m[i][j] = sweep[i][j + MatNum];
            }
        }
    }

public:// 静的メンバ

    // 単位行列
    static Matrix4 Identity() {
        Matrix4 mat;
        mat.InitializeIdentity();
        return mat;
    }

    // 転置行列
    static Matrix4 Transpose(const Matrix4& m) {
        Matrix4 result;
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                result.m[i][j] = m.m[j][i];
            }
        }
        return result;
    }

    static Matrix4 Inverse(const Matrix4& m) {
        const int MatNum = 4; // 行列の次元
        float sweep[MatNum][MatNum * 2] = {};
        Matrix4 result;

        // 初期化：sweepの左側に行列m、右側に単位行列をセット
        for (int i = 0; i < MatNum; i++) {
            for (int j = 0; j < MatNum; j++) {
                sweep[i][j] = m.m[i][j];
                sweep[i][j + MatNum] = (i == j) ? 1.0f : 0.0f;
            }
        }

        // ガウス・ジョルダン法による逆行列計算
        for (int k = 0; k < MatNum; k++) {
            // ピボット選択
            float max = fabs(sweep[k][k]);
            int max_i = k;
            for (int i = k + 1; i < MatNum; i++) {
                if (fabs(sweep[i][k]) > max) {
                    max = fabs(sweep[i][k]);
                    max_i = i;
                }
            }

            // ピボットが小さすぎる場合はエラー（非正則な行列）
            if (fabs(max) < 1e-6) {
                result.InitializeIdentity(); // 逆行列が存在しない場合、単位行列を返す
                return result;
            }

            // 行の入れ替え
            if (k != max_i) {
                for (int j = 0; j < MatNum * 2; j++) {
                    std::swap(sweep[k][j], sweep[max_i][j]);
                }
            }

            // ピボット行を1にスケール
            float pivot = sweep[k][k];
            for (int j = 0; j < MatNum * 2; j++) {
                sweep[k][j] /= pivot;
            }

            // ピボット列の他の要素をゼロにする
            for (int i = 0; i < MatNum; i++) {
                if (i != k) {
                    float factor = sweep[i][k];
                    for (int j = 0; j < MatNum * 2; j++) {
                        sweep[i][j] -= sweep[k][j] * factor;
                    }
                }
            }
        }

        // 結果をMatrix4にコピー
        for (int i = 0; i < MatNum; i++) {
            for (int j = 0; j < MatNum; j++) {
                result.m[i][j] = sweep[i][j + MatNum];
            }
        }
        return result;
    }
};