#pragma once
#include "ChoMath.h"
#include <optional>
struct TransformComponent final
{
	Vector3 translation = { 0.0f, 0.0f, 0.0f };			// 位置
	Quaternion rotation = { 0.0f, 0.0f, 0.0f,1.0f };	// 回転
	Scale scale = { 1.0f, 1.0f, 1.0f };					// スケール
	Matrix4 matWorld = ChoMath::MakeIdentity4x4();		// ワールド行列
	Matrix4 rootMatrix = ChoMath::MakeIdentity4x4();	// ルートのワールド行列
	Vector3 degrees = { 0.0f,0.0f,0.0f };				// 度数表示,操作用変数
	Vector3 prePos = { 0.0f,0.0f,0.0f };				// 位置差分計算用
	Vector3 preRot = { 0.0f,0.0f,0.0f };				// 回転差分計算用
	Scale preScale = { 1.0f,1.0f,1.0f };				// スケール差分計算用
	std::optional<uint32_t> parentEntity = std::nullopt;// 親のEntity
	//uint32_t bufferIndex = UINT32_MAX;				// バッファーインデックス
	std::optional<uint32_t> mapID = std::nullopt;		// マップインデックス
};
// Node用Transform構造体
struct NodeTransform
{
    Vector3 translation = { 0.0f, 0.0f, 0.0f };
    Quaternion rotation = { 0.0f, 0.0f, 0.0f,1.0f };
    Scale scale = { 1.0f, 1.0f, 1.0f };
};

struct CameraComponent final
{
    // 垂直方向視野角
    float fovAngleY = 45.0f * std::numbers::pi_v<float> / 180.0f;
    // ビューポートのアスペクト比
    float aspectRatio = 16.0f / 9.0f;
    // 深度限界（手前側）
    float nearZ = 0.1f;
    // 深度限界（奥側）
    float farZ = 1000.0f;
	// バッファーインデックス
	std::optional<uint32_t> bufferIndex = std::nullopt;
};

// メッシュコンポーネント
struct MeshFilterComponent final
{
	std::optional<uint32_t> modelID = std::nullopt;// Model選択用ID
};

// 描画コンポーネント
struct MeshRendererComponent final
{
	bool visible = true;// 描画フラグ
};