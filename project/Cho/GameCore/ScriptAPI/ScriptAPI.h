#pragma once
#include "Core/Utility/Components.h"

// 不正アクセスを防ぐためのラッパー
template<typename T>
class SafeRef
{
public:
	SafeRef() : ptr_(nullptr), valid_(false) {}

	SafeRef(T& ref) : ptr_(&ref), valid_(true) {}

	// 明示的に無効な参照を作るとき
	static SafeRef Invalid()
	{
		return SafeRef(nullptr, false);
	}

	// アクセス
	T& get()
	{
		if (!valid_ || ptr_ == nullptr)
		{
			//Cho::Log::Write(Cho::LogLevel::Assert, "Tried to access invalid SafeRef!");
			assert(false);
		}
		return *ptr_;
	}

	T& operator*() { return get(); }
	T* operator->() { return &get(); }

	// 有効チェック用
	bool isValid() const { return valid_ && ptr_ != nullptr; }

private:
	SafeRef(T* ptr, bool valid) : ptr_(ptr), valid_(valid) {}

	T* ptr_;
	bool valid_;
};


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