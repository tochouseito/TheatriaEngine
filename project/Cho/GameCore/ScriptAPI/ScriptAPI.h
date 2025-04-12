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

// スクリプトコンテキスト
class ECSManager;
struct ScriptContext
{
	TransformAPI transform;	// TransformAPI
private:
	std::optional<Entity> m_entity = std::nullopt;	// スクリプトのエンティティ
	ECSManager* m_ECS = nullptr;	// ECSManager

	friend class ScriptInitializeSystem;
	friend class ScriptUpdateSystem;
	friend class ScirptFinalizeSystem;

	void InitializeTransformAPI();
public:
	// デフォルトコンストラクタ
	ScriptContext(ECSManager* ecs,std::optional<Entity> entity) :m_ECS(ecs),m_entity(entity) {}
	// コピー、代入禁止
	ScriptContext(const ScriptContext&) = delete;
	ScriptContext& operator=(const ScriptContext&) = delete;
	// ムーブは許可する
	ScriptContext(ScriptContext&&) noexcept = default;
	ScriptContext& operator=(ScriptContext&&) noexcept = default;
};