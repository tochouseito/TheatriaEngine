#pragma once
#include "Core/Utility/Components.h"
#include "Core/Utility/InputStruct.h"

#define REGISTER_SCRIPT_FACTORY(SCRIPTNAME) \
    extern "C" __declspec(dllexport) IScript* Create##SCRIPTNAME##Script() { \
        return new SCRIPTNAME(); \
    }

struct TransformAPI
{
	// 関数
	Vector3& position() { return data->translation; }
	Vector3& rotation() { return data->degrees; }
	Scale& scale() { return data->scale; }
	std::function<void(Entity, const Vector3&)> SetPosition;
	std::function<Vector3(Entity)> GetPosition;
private:
	friend struct ScriptContext;
	TransformComponent* data = nullptr;
};

struct CameraAPI
{
	// 関数
	float& fovAngleY() { return data->fovAngleY; }
	float& aspectRatio() { return data->aspectRatio; }
	float& nearZ() { return data->nearZ; }
	float& farZ() { return data->farZ; }
private:
	friend struct ScriptContext;
	CameraComponent* data = nullptr;
};

struct LineRendererAPI
{
	// 関数
	Vector3& start(uint32_t index) { return (*data)[index].line.start; }
	Vector3& end(uint32_t index) { return (*data)[index].line.end; }
	Color& color(uint32_t index) { return (*data)[index].line.color; }
private:
	friend struct ScriptContext;
	std::vector<LineRendererComponent>* data = nullptr;
};

struct Rigidbody2DAPI
{
	// 関数
	class RayCastCallback : public b2RayCastCallback
	{
	public:
		bool hit = false;
		b2Vec2 point;
		b2Vec2 normal;
		float fraction = 1.0f;

		float ReportFixture(b2Fixture* a_Fixture, const b2Vec2& a_Point,
			const b2Vec2& a_Normal, float a_Fraction) override
		{
			a_Fixture;
			this->hit = true;
			this->point = a_Point;
			this->normal = a_Normal;
			this->fraction = a_Fraction;
			return a_Fraction; // 最も近いヒットのみ取得
		}
	private:
	};
	// 反射方向を計算
	std::function<b2Vec2(const b2Vec2& incident, const b2Vec2& normal)> Reflect;
	std::function<b2Vec2(const b2Vec2& start, const b2Vec2& dir,const int ReflectionCount, const float maxLength)> RaycastWithReflectionsOnce;
	// 法線取得（RaycastOnce の直後のみ有効）
	b2Vec2 GetLastHitNormal() const { return m_LastHitNormal; }
private:
	friend struct ScriptContext;
	Rigidbody2DComponent* data = nullptr;
	// 最後の法線（内部的に保持、ただし状態は保持しないなら静的でもよい）
	b2Vec2 m_LastHitNormal = b2Vec2(0.0f, 1.0f); // 一時的な用途
};

//InputAPI
class InputManager;
struct InputAPI
{
	// 関数
	// キーの押下をチェック
	std::function<bool(const uint8_t& keyNumber)> PushKey;
	// キーのトリガーをチェック
	std::function<bool(const uint8_t& keyNumber)> TriggerKey;
	// 全マウス情報取得
	std::function<const DIMOUSESTATE2& ()> GetAllMouse;
	// マウス移動量を取得
	std::function<MouseMove()> GetMouseMove;
	// マウスの押下をチェック
	std::function<bool(const int32_t& mouseNumber)> IsPressMouse;
	// マウスのトリガーをチェック。押した瞬間だけtrueになる
	std::function<bool(const int32_t& buttonNumber)> IsTriggerMouse;
	// マウスの位置を取得する（ウィンドウ座標系）
	std::function<const Vector2& ()> GetMouseWindowPosition;
	// マウスの位置を取得する（スクリーン座標系）
	std::function<Vector2()> GetMouseScreenPosition;
	// 現在のジョイスティック状態を取得する
	std::function<bool(const int32_t& stickNo, XINPUT_STATE& out)> GetJoystickState;
	// 前回のジョイスティック状態を取得する
	std::function<bool(const int32_t& stickNo, XINPUT_STATE& out)> GetJoystickStatePrevious;
	// デッドゾーンを設定する
	std::function<void(const int32_t& stickNo, const int32_t& deadZoneL, const int32_t& deadZoneR)> SetJoystickDeadZone;
	// 接続されているジョイスティック数を取得する
	std::function<size_t()> GetNumberOfJoysticks;
	// パッドの押されているボタン、スティックの値を取得
	std::function<bool(const PadButton& button, int32_t stickNo)> IsTriggerPadButton;
	std::function<bool(const PadButton& button, int32_t stickNo)> IsPressPadButton;
	std::function<Vector2(const LR& padStick, int32_t stickNo)> GetStickValue;
	std::function<float(const LR& LorR, int32_t stickNo)> GetLRTrigger;
	
private:
	friend struct ScriptContext;
	InputManager* data = nullptr;
};
// スクリプトコンテキスト
class ECSManager;
class ResourceManager;
class ObjectContainer;
struct ScriptContext
{
public:
	TransformAPI transform;	// TransformAPI
	CameraAPI camera;	// CameraAPI
	LineRendererAPI lineRenderer;	// LineRendererAPI
	Rigidbody2DAPI rigidbody2D;	// Rigidbody2DAPI

	// Input
	InputAPI input;	// InputAPI
private:
	std::optional<Entity> m_Entity = std::nullopt;	// スクリプトのエンティティ
	ECSManager* m_ECS = nullptr;	// ECSManager
	ResourceManager* m_ResourceManager = nullptr;	// ResourceManager
	InputManager* m_InputManager = nullptr;	// InputManager
	ObjectContainer* m_ObjectContainer = nullptr;	// ObjectContainer

	friend class ScriptInitializeSystem;
	friend class ScriptUpdateSystem;
	friend class ScirptFinalizeSystem;
	friend class CollisionSystem;
	friend class ContactListener2D;

	void Initialize()
	{
		InitializeTransformAPI();
		InitializeCameraAPI();
		InitializeLineRendererAPI();
		InitializeRigidbody2DAPI();
		InitializeInputAPI();
	}

	void InitializeTransformAPI();
	void InitializeCameraAPI();
	void InitializeLineRendererAPI();
	void InitializeRigidbody2DAPI();
	void InitializeInputAPI();
public:
	// デフォルトコンストラクタ
	ScriptContext(ObjectContainer* objectContainer,InputManager* input,ResourceManager* resourceManager, ECSManager* ecs, std::optional<Entity> entity) :m_ObjectContainer(objectContainer), m_InputManager(input), m_ResourceManager(resourceManager), m_ECS(ecs), m_Entity(entity) {}
	// コピー、代入禁止
	ScriptContext(const ScriptContext&) = delete;
	ScriptContext& operator=(const ScriptContext&) = delete;
	// ムーブは許可する
	ScriptContext(ScriptContext&&) noexcept = default;
	ScriptContext& operator=(ScriptContext&&) noexcept = default;
};