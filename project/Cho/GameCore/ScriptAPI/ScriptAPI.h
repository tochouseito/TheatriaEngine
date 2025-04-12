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
	
private:
	friend struct ScriptContext;
	std::vector<LineRendererComponent>* data = nullptr;
};

// スクリプトコンテキスト
class ECSManager;
class ResourceManager;
struct ScriptContext
{
public:
	TransformAPI transform;	// TransformAPI
	CameraAPI camera;	// CameraAPI

private:
	std::optional<Entity> m_Entity = std::nullopt;	// スクリプトのエンティティ
	ECSManager* m_ECS = nullptr;	// ECSManager
	ResourceManager* m_ResourceManager = nullptr;	// ResourceManager

	friend class ScriptInitializeSystem;
	friend class ScriptUpdateSystem;
	friend class ScirptFinalizeSystem;

	void InitializeTransformAPI();
public:
	// デフォルトコンストラクタ
	ScriptContext(ResourceManager* resourceManager,ECSManager* ecs,std::optional<Entity> entity) :m_ResourceManager(resourceManager), m_ECS(ecs), m_Entity(entity) {}
	// コピー、代入禁止
	ScriptContext(const ScriptContext&) = delete;
	ScriptContext& operator=(const ScriptContext&) = delete;
	// ムーブは許可する
	ScriptContext(ScriptContext&&) noexcept = default;
	ScriptContext& operator=(ScriptContext&&) noexcept = default;
};