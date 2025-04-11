#pragma once
#include "ChoMath.h"
#include "Core/Utility/Color.h"
#include <vector>         // C++98
#include <array>          // C++11
#include <functional>     // C++98
#include <bitset>         // C++98
#include <memory>         // C++98
#include <algorithm>      // C++98
#include <unordered_map>  // C++11
#include <unordered_set>  // C++11
#include <typeindex>      // C++11
#include <optional>       // C++17
#include <concepts>       // C++20
#include <ranges>         // C++20

using Entity = uint32_t;
using CompID = size_t;
using Archetype = std::bitset<256>;
struct ScriptContext;

// コンポーネントだと判別するためのタグ
struct IComponentTag {};
// コンポーネントが複数持てるか(デフォルトは持てない)
template<typename T>
struct IsMultiComponent : std::false_type {};

// 初期値を保存するための構造体
struct TransformStartValue
{
	Vector3 translation = { 0.0f, 0.0f, 0.0f };
	Quaternion rotation = { 0.0f, 0.0f, 0.0f,1.0f };
	Scale scale = { 1.0f, 1.0f, 1.0f };
	Vector3 degrees = { 0.0f, 0.0f, 0.0f };
};;

struct TransformComponent : public IComponentTag
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
	std::optional<uint32_t> parent = std::nullopt;// 親のEntity
	int tickPriority = 0;								// Tick優先度
	//uint32_t bufferIndex = UINT32_MAX;				// バッファーインデックス
	std::optional<uint32_t> mapID = std::nullopt;		// マップインデックス
	TransformStartValue startValue;						// 初期値保存用
};
// Node用Transform構造体
struct NodeTransform
{
    Vector3 translation = { 0.0f, 0.0f, 0.0f };
    Quaternion rotation = { 0.0f, 0.0f, 0.0f,1.0f };
    Scale scale = { 1.0f, 1.0f, 1.0f };
};

struct CameraComponent : public IComponentTag
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
struct MeshFilterComponent : public IComponentTag
{
	std::wstring modelName = L"";// モデル名
	std::optional<uint32_t> modelID = std::nullopt;// Model選択用ID
};

// 描画コンポーネント
struct MeshRendererComponent : public IComponentTag
{
	bool visible = true;// 描画フラグ
};

// スクリプトコンポーネント
struct ScriptComponent : public IComponentTag
{
	std::string scriptName = "";								// スクリプト名
	std::optional<uint32_t> scriptID = std::nullopt;			// スクリプトID
	std::optional<Entity> entity = std::nullopt;				// スクリプトのエンティティ
	using ScriptFunc = std::function<void(ScriptContext& ctx)>; // スクリプト関数型
	ScriptFunc startFunc;										// Start関数
	ScriptFunc updateFunc;										// Update関数
	std::function<void()> cleanupFunc;							// 解放関数
	bool isActive = false;										// スクリプト有効フラグ
};

// ライン描画コンポーネント
struct LineData
{
	Vector3 start;	// 始点
	Vector3 end;	// 終点
	Color color;	// 色
	std::optional<uint32_t> lineID = std::nullopt;// ラインID
};

struct LineRendererComponent : public IComponentTag
{
	std::vector<LineData> lines;// ラインデータ
	std::optional<uint32_t> mapID = std::nullopt;		// マップインデックス
};

// マルチコンポーネントを許可
template<>
struct IsMultiComponent<LineRendererComponent> : std::true_type {};