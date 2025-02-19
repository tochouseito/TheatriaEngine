#pragma once

// Components
#include"ECS/ComponentManager/Components/Components.h"

// EntityManager
#include "ECS/EntityManager/EntityManager.h"

// C++
#include <unordered_map>

class ResourceViewManager;
// 各エンティティに対してコンポーネントを管理するクラス
//template<typename T>
class ComponentManager {
public:
    ~ComponentManager() = default;

    void SetRVManager(ResourceViewManager* RVManager);

	// エンティティにコンポーネントを追加します。
	template<typename T, typename = EnableIfComponent<T>>
	void AddComponent(const Entity& entity, const T& component) {
		auto& components = GetOrCreateComponentMap<T>();
		components[entity] = component;

		// 特殊処理
		SpecialProcess(entity, component);
	}

	template<typename T, typename = EnableIfComponent<T>>
	std::optional<std::reference_wrapper<T>> GetComponent(const Entity& entity) {
		auto& components = GetOrCreateComponentMap<T>();
		auto it = components.find(entity);

		if (it == components.end()) {
			return std::nullopt; // コンポーネントが見つからなかった場合
		}

		return std::ref(it->second); // コンポーネントの参照を返す
	}

    // 指定されたエンティティからコンポーネントを削除します。
    void RemoveComponent(Entity entity);

	// 特殊処理

	// TransformComponentの特殊処理
	void RemoveSpecialProcess(const Entity& entity, const TransformComponent& component);

	// CameraComponentの特殊処理
	void RemoveSpecialProcess(const Entity& entity, const CameraComponent& component);

	// MaterialComponentの特殊処理
	void RemoveSpecialProcess(const Entity& entity, const MaterialComponent& component);

	// SpriteComponentの特殊処理
	void RemoveSpecialProcess(const Entity& entity, const SpriteComponent& component);

	// ParticleComponentの特殊処理
	void RemoveSpecialProcess(const Entity& entity, const ParticleComponent& component);

	// EmitterComponentの特殊処理
	void RemoveSpecialProcess(const Entity& entity, const EmitterComponent& component);

	// EffectComponentの特殊処理
	void RemoveSpecialProcess(const Entity& entity, const EffectComponent& component);

	// DirectionalLightComponentの特殊処理
	void RemoveSpecialProcess(const Entity& entity, const DirectionalLightComponent& component);

	// PointLightComponentの特殊処理
	void RemoveSpecialProcess(const Entity& entity, const PointLightComponent& component);

	// SpotLightComponentの特殊処理
	void RemoveSpecialProcess(const Entity& entity, const SpotLightComponent& component);

	

	template<typename T>
	void RemoveSpecialProcess(const Entity& entity, const T& component) {
		// デフォルトの特殊処理
		entity;
		component;
		// 何もしない
	}

    // コンポーネント初期化関数
    void InitCameraComponent(CameraComponent* camera);

private:

	template<typename T, typename=EnableIfComponent<T>>
	std::unordered_map<Entity, T>& GetOrCreateComponentMap() {
		std::type_index typeIndex = (typeid(T));

		if (componentMap.find(typeIndex) == componentMap.end()) {
			componentMap[typeIndex] = std::make_shared<std::unordered_map<Entity, T>>();
		}

		return *std::static_pointer_cast<std::unordered_map<Entity, T>>(componentMap[typeIndex]);
	}

    // 特殊処理
	
	// TransformComponentの特殊処理
	void SpecialProcess(const Entity& entity, const TransformComponent& component);

	// CameraComponentの特殊処理
	void SpecialProcess(const Entity& entity, const CameraComponent& component);

	// MaterialComponentの特殊処理
	void SpecialProcess(const Entity& entity, const MaterialComponent& component);

	// SpriteComponentの特殊処理
	void SpecialProcess(const Entity& entity, const SpriteComponent& component);

	// ParticleComponentの特殊処理
	void SpecialProcess(const Entity& entity, const ParticleComponent& component);

	// EmitterComponentの特殊処理
	void SpecialProcess(const Entity& entity, const EmitterComponent& component);

	// EffectComponentの特殊処理
	void SpecialProcess(const Entity& entity, const EffectComponent& component);

	// DirectionalLightComponentの特殊処理
	void SpecialProcess(const Entity& entity, const DirectionalLightComponent& component);

	// PointLightComponentの特殊処理
	void SpecialProcess(const Entity& entity, const PointLightComponent& component);

	// SpotLightComponentの特殊処理
	void SpecialProcess(const Entity& entity, const SpotLightComponent& component);

    // デフォルトの特殊処理
	template<typename T>
    void SpecialProcess(const Entity& entity, const T& component)
	{
		// デフォルトの特殊処理
		entity;
		component;
		// 何もしない
	}
    
private:
	std::unordered_map<std::type_index, std::shared_ptr<void>> componentMap;

    ResourceViewManager* rvManager_ = nullptr;
};
