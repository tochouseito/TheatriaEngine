#pragma once
#include "ChoMath.h"// ChoEngine数学ライブラリ
#include <Externals/box2d/include/box2d/box2d.h>
#include "Core/Utility/Color.h"
#include "Core/Utility/IDType.h"
#include "Core/Utility/EffectStruct.h"
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
#include <numbers>        // C++20
#include <cstdint>
#include <variant>

//struct ScriptContext;
class GameObject;

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
	std::optional<uint32_t> parent = std::nullopt;		// 親のEntity
	int tickPriority = 0;								// Tick優先度
	//uint32_t bufferIndex = UINT32_MAX;				// バッファーインデックス
	std::optional<uint32_t> mapID = std::nullopt;		// マップインデックス
	TransformStartValue startValue;						// 初期値保存用
	std::optional<uint32_t> materialID = std::nullopt;	// マテリアルID
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
	std::optional<ObjectID> objectID = std::nullopt;	// スクリプトのオブジェクトID
	//std::optional<Entity> entity = std::nullopt;				// スクリプトのエンティティ
	using ScriptFunc = std::function<void()>; // スクリプト関数型

	ScriptFunc startFunc;										// Start関数
	ScriptFunc updateFunc;										// Update関数
	std::function<void()> cleanupFunc;							// 解放関数

	std::function<void(GameObject& other)> onCollisionEnterFunc;	// 衝突開始関数
	std::function<void(GameObject& other)> onCollisionStayFunc;	// 衝突中関数
	std::function<void(GameObject& other)> onCollisionExitFunc;	// 衝突終了関数

	bool isActive = false;										// スクリプト有効フラグ
};

// ライン描画コンポーネント
struct LineData
{
	Vector3 start;	// 始点
	Vector3 end;	// 終点
	Color color;	// 色
};

struct LineRendererComponent : public IComponentTag
{
	LineData line;// ラインデータ
	std::optional<uint32_t> mapID = std::nullopt;		// マップインデックス
};

// 2D物理コンポーネント
// 物理エンジンに統合予定
// 2D物理挙動コンポーネント
struct Rigidbody2DComponent : public IComponentTag
{
	bool isActive = true; // 有効フラグ
	float mass = 1.0f;
	float gravityScale = 1.0f;
	bool isKinematic = false;
	bool fixedRotation = false;
	b2BodyType bodyType = b2_dynamicBody;
	b2Body* runtimeBody = nullptr; // Box2D Bodyへのポインタ
	b2World* world = nullptr; // Box2D Worldへのポインタ
	bool isCollisionStay = false; // 衝突中フラグ
	std::optional<ObjectID> otherObjectID = std::nullopt; // 衝突したオブジェクトID
	std::optional<ObjectID> selfObjectID = std::nullopt; // 自分のオブジェクトID
	std::optional<b2Vec2> requestedPosition = std::nullopt; // 位置リクエスト
	//std::optional<b2Vec2> requestedVelocity = std::nullopt; // 速度リクエスト
	Vector2 velocity = { 0.0f, 0.0f }; // 速度
};
// 2D矩形コライダー
struct BoxCollider2DComponent : public IComponentTag
{
	float offsetX = 0.0f;
	float offsetY = 0.0f;
	float width = 1.0f;
	float height = 1.0f;
	float density = 1.0f;
	float friction = 0.3f;
	float restitution = 0.0f;
	bool isSensor = false;
	bool isActive = true;
	b2Fixture* runtimeFixture = nullptr;
};
// 2D円形コライダー
struct CircleCollider2DComponent : public IComponentTag
{
	float offsetX = 0.0f;
	float offsetY = 0.0f;
	float radius = 0.5f;
	float density = 1.0f;
	float friction = 0.3f;
	float restitution = 0.0f;
	b2Fixture* runtimeFixture = nullptr;
};
// 2D任意形状コライダー
struct PolygonCollider2DComponent : public IComponentTag
{
	std::vector<b2Vec2> points;
	float density = 1.0f;
	float friction = 0.3f;
	float restitution = 0.0f;
	b2Fixture* runtimeFixture = nullptr;
};
// マテリアルコンポーネント
struct MaterialComponent : public IComponentTag
{
	Color color;
	bool enableLighting = true;
	bool enableTexture = false;
	Matrix4 matUV;
	float shininess;
	std::wstring textureName = L"";	// テクスチャ名
	std::optional<uint32_t> textureID = std::nullopt;	// テクスチャID
	std::optional<uint32_t> mapID = std::nullopt;
};
// エミッターコンポーネント
struct EmitterComponent : public IComponentTag
{
	Vector3 position = { 0.0f,0.0f,0.0f };	// 位置
	float radius = 1.0f;					// 射出半径
	uint32_t count = 10;					// 射出数
	float frequency = 0.5f;					// 射出間隔
	float frequencyTime = 0.0f;				// 射出間隔調整用時間
	uint32_t emit = 0;						// 射出許可
	std::optional<uint32_t> particleID = std::nullopt;	// パーティクルID
	// バッファインデックス
	std::optional<uint32_t> bufferIndex = std::nullopt;
};
// パーティクルコンポーネント
struct ParticleComponent : public IComponentTag
{
	uint32_t count = 1024;// パーティクル数
	Matrix4 matBillboard = ChoMath::MakeIdentity4x4();
	float time = 0.0f;
	float deltaTime = 0.0f;
	// バッファーインデックス
	std::optional<uint32_t> bufferIndex = std::nullopt;
	std::optional<uint32_t> perFrameBufferIndex = std::nullopt;
	std::optional<uint32_t> freeListIndexBufferIndex = std::nullopt;
	std::optional<uint32_t> freeListBufferIndex = std::nullopt;
};
// エフェクトコンポーネント
struct EffectComponent : public IComponentTag
{
	std::wstring effectName = L"";	// エフェクト名
	std::optional<uint32_t> effectID = std::nullopt;	// エフェクトID
	float globalTime = 0.0f;	// グローバル時間
	float maxTime = 120.0f;	// 最大時間
	float deltaTime = 0.0f;	// デルタ時間
	bool isRun = false;	// 実行フラグ
	bool isPreRun = false;	// プリセット実行フラグ
	bool isLoop = true;	// ループフラグ
	bool isReset = false;	// リセットフラグ
	std::vector<uint32_t> nodeID;	// ノードID
	std::vector<EffectNodeData> nodeData;	// ノードデータ

};
// スプライトコンポーネント
struct UISpriteComponent : public IComponentTag
{
	Vector2 position{ 0.0f,0.0f };// 位置
	float rotation = 0.0f;// Z軸回転
	Vector2 scale = { 1.0f,1.0f };// スケール
	Matrix4 matWorld = ChoMath::MakeIdentity4x4();
	Vector2 uvPos = { 0.0f,0.0f };
	float uvRot = 0.0f;
	Vector2 uvScale = { 1.0f,1.0f };
	Vector2 anchorPoint = { 0.0f,0.0f };// アンカーポイント
	Vector2 size = { 64.0f,64.0f };// サイズ
	Vector2 textureLeftTop = { 0.0f,0.0f };// テクスチャの左上座標
	Vector2 textureSize = { 64.0f,64.0f };// テクスチャのサイズ

	std::optional<uint32_t> mapID = std::nullopt;
};

// マルチコンポーネントを許可
template<>
struct IsMultiComponent<LineRendererComponent> : std::true_type {};

// オブジェクトタイプによるコンポーネントの所持許可
// デフォルト : すべてfalse
template<ObjectType T,typename Component>
constexpr bool IsComponentAllowed = false;

// MeshObject
template<> constexpr bool IsComponentAllowed<ObjectType::MeshObject, TransformComponent> = true;
template<> constexpr bool IsComponentAllowed<ObjectType::MeshObject, MeshFilterComponent> = true;
template<> constexpr bool IsComponentAllowed<ObjectType::MeshObject, MeshRendererComponent> = true;
template<> constexpr bool IsComponentAllowed<ObjectType::MeshObject, ScriptComponent> = true;
template<> constexpr bool IsComponentAllowed<ObjectType::MeshObject, MaterialComponent> = true;
template<> constexpr bool IsComponentAllowed<ObjectType::MeshObject, LineRendererComponent> = true;
template<> constexpr bool IsComponentAllowed<ObjectType::MeshObject, Rigidbody2DComponent> = true;
template<> constexpr bool IsComponentAllowed<ObjectType::MeshObject, BoxCollider2DComponent> = true;

// Camera
template<> constexpr bool IsComponentAllowed<ObjectType::Camera, TransformComponent> = true;
template<> constexpr bool IsComponentAllowed<ObjectType::Camera, CameraComponent> = true;
template<> constexpr bool IsComponentAllowed<ObjectType::Camera, ScriptComponent> = true;

// ParticleSystem
template<> constexpr bool IsComponentAllowed<ObjectType::ParticleSystem, TransformComponent> = true;
template<> constexpr bool IsComponentAllowed<ObjectType::ParticleSystem, EmitterComponent> = true;
template<> constexpr bool IsComponentAllowed<ObjectType::ParticleSystem, ParticleComponent> = true;

// Effect
template<> constexpr bool IsComponentAllowed<ObjectType::Effect, TransformComponent> = true;
template<> constexpr bool IsComponentAllowed<ObjectType::Effect, EffectComponent> = true;

// UI
template<> constexpr bool IsComponentAllowed<ObjectType::UI, TransformComponent> = true;
template<> constexpr bool IsComponentAllowed<ObjectType::UI, UISpriteComponent> = true;
template<> constexpr bool IsComponentAllowed<ObjectType::UI, ScriptComponent> = true;

// ランタイム対応
template<typename Component>
bool IsComponentAllowedAtRuntime(ObjectType type)
{
	switch (type)
	{
	case ObjectType::MeshObject:
		return IsComponentAllowed<ObjectType::MeshObject, Component>;
	case ObjectType::Camera:
		return IsComponentAllowed<ObjectType::Camera, Component>;
	case ObjectType::ParticleSystem:
		return IsComponentAllowed<ObjectType::ParticleSystem, Component>;
	case ObjectType::Effect:
		return IsComponentAllowed<ObjectType::Effect, Component>;
	case ObjectType::UI:
		return IsComponentAllowed<ObjectType::UI, Component>;
	case ObjectType::Count:
		return false;
	default:
		return false;
	}
}