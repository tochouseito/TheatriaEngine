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

	TransformComponent& operator=(const TransformComponent& other)
	{
		if (this == &other) return *this;

		translation = other.translation;
		rotation = other.rotation;
		scale = other.scale;
		degrees = other.degrees;
		prePos = other.prePos;
		preRot = other.preRot;
		preScale = other.preScale;
		tickPriority = other.tickPriority;

		// optionalな要素は、コピー可能な値として処理
		parent = other.parent;
		//mapID = other.mapID;
		//materialID = other.materialID;

		// ワールド行列やルート行列は差分として扱わない場合はコピーしない
		// matWorld = other.matWorld;
		// rootMatrix = other.rootMatrix;

		// startValue も含めたい場合はこちらを有効に
		//startValue = other.startValue;

		return *this;
	}

	// 初期化
	void Initialize()
	{
		translation.Initialize();
		rotation.Initialize();
		scale.Initialize();
		matWorld = ChoMath::MakeIdentity4x4();
		rootMatrix = ChoMath::MakeIdentity4x4();
		degrees.Initialize();
		prePos.Initialize();
		preRot.Initialize();
		preScale.Initialize();
		tickPriority = 0;
		parent = std::nullopt;
		mapID = std::nullopt;
		materialID = std::nullopt;
	}
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

	CameraComponent& operator=(const CameraComponent& other)
	{
		if (this == &other) return *this;
		fovAngleY = other.fovAngleY;
		aspectRatio = other.aspectRatio;
		nearZ = other.nearZ;
		farZ = other.farZ;
		return *this;
	}
	// 初期化
	void Initialize()
	{
		fovAngleY = 45.0f * std::numbers::pi_v<float> / 180.0f;
		aspectRatio = 16.0f / 9.0f;
		nearZ = 0.1f;
		farZ = 1000.0f;
	}
};

// メッシュコンポーネント
struct MeshFilterComponent : public IComponentTag
{
	std::wstring modelName = L"";// モデル名
	std::optional<uint32_t> modelID = std::nullopt;// Model選択用ID

	MeshFilterComponent& operator=(const MeshFilterComponent& other)
	{
		if (this == &other) return *this;
		modelName = other.modelName;
		return *this;
	}

	// 初期化
	void Initialize()
	{
		modelName = L"";
		modelID = std::nullopt;
	}
};

// 描画コンポーネント
struct MeshRendererComponent : public IComponentTag
{
	bool visible = true;// 描画フラグ

	MeshRendererComponent& operator=(const MeshRendererComponent& other)
	{
		if (this == &other) return *this;
		visible = other.visible;
		return *this;
	}

	// 初期化
	void Initialize()
	{
		visible = true;
	}
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

	bool isActive = false;				// スクリプト有効フラグ

	ScriptComponent& operator=(const ScriptComponent& other)
	{
		if (this == &other) return *this;
		scriptName = other.scriptName;
		return *this;
	}

	// 初期化
	void Initialize()
	{
		scriptName = "";
		objectID = std::nullopt;
		startFunc = nullptr;
		updateFunc = nullptr;
		cleanupFunc = nullptr;
		onCollisionEnterFunc = nullptr;
		onCollisionStayFunc = nullptr;
		onCollisionExitFunc = nullptr;
		isActive = false;
	}
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

	LineRendererComponent& operator=(const LineRendererComponent& other)
	{
		if (this == &other) return *this;
		line = other.line;
		return *this;
	}

	// 初期化
	void Initialize()
	{
		line.start.Initialize();
		line.end.Initialize();
		line.color.Initialize();
		mapID = std::nullopt;
	}
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

	Rigidbody2DComponent& operator=(const Rigidbody2DComponent& other)
	{
		if (this == &other) return *this;
		isActive = other.isActive;
		mass = other.mass;
		gravityScale = other.gravityScale;
		isKinematic = other.isKinematic;
		fixedRotation = other.fixedRotation;
		bodyType = other.bodyType;
		return *this;
	}

	// 初期化
	void Initialize()
	{
		if (runtimeBody)
		{
			world->DestroyBody(runtimeBody);
			runtimeBody = nullptr;
		}
		isActive = true;
		mass = 1.0f;
		gravityScale = 1.0f;
		isKinematic = false;
		fixedRotation = false;
		bodyType = b2_dynamicBody;
		world = nullptr;
		isCollisionStay = false;
		otherObjectID = std::nullopt;
		selfObjectID = std::nullopt;
		requestedPosition = std::nullopt;
		velocity.Initialize();
	}
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

	BoxCollider2DComponent& operator=(const BoxCollider2DComponent& other)
	{
		if (this == &other) return *this;
		offsetX = other.offsetX;
		offsetY = other.offsetY;
		width = other.width;
		height = other.height;
		density = other.density;
		friction = other.friction;
		restitution = other.restitution;
		isSensor = other.isSensor;
		return *this;
	}

	// 初期化
	void Initialize()
	{
		offsetX = 0.0f;
		offsetY = 0.0f;
		width = 1.0f;
		height = 1.0f;
		density = 1.0f;
		friction = 0.3f;
		restitution = 0.0f;
		isSensor = false;
		isActive = true;
		runtimeFixture = nullptr;
	}
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
	bool uvFlipY = false;
	Matrix4 matUV;
	float shininess = 50.0f;
	std::wstring textureName = L"";	// テクスチャ名
	std::optional<uint32_t> textureID = std::nullopt;	// テクスチャID
	std::optional<uint32_t> mapID = std::nullopt;

	MaterialComponent& operator=(const MaterialComponent& other)
	{
		if (this == &other) return *this;
		color = other.color;
		enableLighting = other.enableLighting;
		enableTexture = other.enableTexture;
		shininess = other.shininess;
		textureName = other.textureName;
		return *this;
	}

	// 初期化
	void Initialize()
	{
		color.Initialize();
		enableLighting = true;
		enableTexture = false;
		uvFlipY = false;
		matUV = ChoMath::MakeIdentity4x4();
		shininess = 0.0f;
		textureName = L"";
		textureID = std::nullopt;
		mapID = std::nullopt;
	}
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

	EmitterComponent& operator=(const EmitterComponent& other)
	{
		if (this == &other) return *this;
		position = other.position;
		radius = other.radius;
		count = other.count;
		frequency = other.frequency;
		frequencyTime = other.frequencyTime;
		return *this;
	}

	// 初期化
	void Initialize()
	{
		position.Initialize();
		radius = 1.0f;
		count = 10;
		frequency = 0.5f;
		frequencyTime = 0.0f;
		emit = 0;
		particleID = std::nullopt;
		bufferIndex = std::nullopt;
	}
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

	ParticleComponent& operator=(const ParticleComponent& other)
	{
		if (this == &other) return *this;
		count = other.count;
		time = other.time;
		deltaTime = other.deltaTime;
		return *this;
	}

	// 初期化
	void Initialize()
	{
		count = 1024;
		matBillboard = ChoMath::MakeIdentity4x4();
		time = 0.0f;
		deltaTime = 0.0f;
		bufferIndex = std::nullopt;
		perFrameBufferIndex = std::nullopt;
		freeListIndexBufferIndex = std::nullopt;
		freeListBufferIndex = std::nullopt;
	}
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

	UISpriteComponent& operator=(const UISpriteComponent& other)
	{
		if (this == &other) return *this;
		position = other.position;
		rotation = other.rotation;
		scale = other.scale;
		//uvPos = other.uvPos;
		//uvRot = other.uvRot;
		//uvScale = other.uvScale;
		anchorPoint = other.anchorPoint;
		size = other.size;
		textureLeftTop = other.textureLeftTop;
		textureSize = other.textureSize;
		return *this;
	}
	// 初期化
	void Initialize()
	{
		position.Initialize();
		rotation = 0.0f;
		scale.Initialize();
		matWorld = ChoMath::MakeIdentity4x4();
		uvPos.Initialize();
		uvRot = 0.0f;
		uvScale.Initialize();
		anchorPoint.Initialize();
		size.Initialize();
		textureLeftTop.Initialize();
		textureSize.Initialize();
		mapID = std::nullopt;
	}
};

// ライトコンポーネント
struct LightComponent : public IComponentTag
{
	Color color;		// 色
	float intensity=1.0f;	// 強度
	float range=10.0f;		// 適用距離
	float decay=1.0f;		// 減衰率
	float spotAngle = 45.0f;	// スポットライトの角度
	float spotFalloffStart = 0.0f;
	LightType type = LightType::Directional;		// ライトの種類
	bool active = true;	// ライトの有効無効
	std::optional<uint32_t> mapID = std::nullopt;	// マップインデックス

	LightComponent& operator=(const LightComponent& other)
	{
		if (this == &other) return *this;
		color = other.color;
		intensity = other.intensity;
		range = other.range;
		decay = other.decay;
		spotAngle = other.spotAngle;
		type = other.type;
		active = other.active;
		return *this;
	}
	// 初期化
	void Initialize()
	{
		color.Initialize();
		intensity = 1.0f;
		range = 10.0f;
		decay = 1.0f;
		spotAngle = 45.0f;
		spotFalloffStart = 0.0f;
		type = LightType::Directional;
		active = true;
	}
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
template<> constexpr bool IsComponentAllowed<ObjectType::UI, MaterialComponent> = true;

// Light
template<> constexpr bool IsComponentAllowed<ObjectType::Light, TransformComponent> = true;
template<> constexpr bool IsComponentAllowed<ObjectType::Light, LightComponent> = true;
template<> constexpr bool IsComponentAllowed<ObjectType::Light, ScriptComponent> = true;

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
	case ObjectType::Light:
		return IsComponentAllowed<ObjectType::Light, Component>;
	case ObjectType::Count:
		return false;
	default:
		return false;
	}
}