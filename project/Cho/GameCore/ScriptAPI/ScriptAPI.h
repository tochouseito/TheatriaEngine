#pragma once
#include "Core/Utility/Components.h"

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
	std::function<b2Vec2(const b2Vec2&, const b2Vec2&)> Reflect;
	std::function<b2Vec2(const b2Vec2&, const b2Vec2&,const int, const float)> RaycastWithReflectionsOnce;
	// 法線取得（RaycastOnce の直後のみ有効）
	b2Vec2 GetLastHitNormal() const { return m_LastHitNormal; }
private:
	friend struct ScriptContext;
	Rigidbody2DComponent* data = nullptr;
	// 最後の法線（内部的に保持、ただし状態は保持しないなら静的でもよい）
	b2Vec2 m_LastHitNormal = b2Vec2(0.0f, 1.0f); // 一時的な用途
};

// スクリプトコンテキスト
class ECSManager;
class ResourceManager;
struct ScriptContext
{
public:
	TransformAPI transform;	// TransformAPI
	CameraAPI camera;	// CameraAPI
	LineRendererAPI lineRenderer;	// LineRendererAPI
	Rigidbody2DAPI rigidbody2D;	// Rigidbody2DAPI

	// LineRendererComponentを追加

private:
	std::optional<Entity> m_Entity = std::nullopt;	// スクリプトのエンティティ
	ECSManager* m_ECS = nullptr;	// ECSManager
	ResourceManager* m_ResourceManager = nullptr;	// ResourceManager

	friend class ScriptInitializeSystem;
	friend class ScriptUpdateSystem;
	friend class ScirptFinalizeSystem;

	void Initialize()
	{
		InitializeTransformAPI();
		InitializeCameraAPI();
		InitializeLineRendererAPI();
		InitializeRigidbody2DAPI();
	}

	void InitializeTransformAPI();
	void InitializeCameraAPI();
	void InitializeLineRendererAPI();
	void InitializeRigidbody2DAPI();
public:
	// デフォルトコンストラクタ
	ScriptContext(ResourceManager* resourceManager, ECSManager* ecs, std::optional<Entity> entity) :m_ResourceManager(resourceManager), m_ECS(ecs), m_Entity(entity) {}
	// コピー、代入禁止
	ScriptContext(const ScriptContext&) = delete;
	ScriptContext& operator=(const ScriptContext&) = delete;
	// ムーブは許可する
	ScriptContext(ScriptContext&&) noexcept = default;
	ScriptContext& operator=(ScriptContext&&) noexcept = default;
};