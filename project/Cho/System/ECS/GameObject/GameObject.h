#pragma once

// ComponentManager
#include "ECS/ComponentManager/ComponentManager.h"

// C++
#include <string>
#include <optional>

#include "SystemState/SystemState.h"

class GameObject {

public:
	GameObject() = default;

	// デストラクタ
	~GameObject();

	void CreateEntity();

	void SetManager(EntityManager* em, ComponentManager* cm);

	// 親オブジェクトを設定します。
	void SetParent(GameObject* newParent);

	// 子オブジェクトを追加します。
	void AddChild(GameObject* child);

	// EntityのIDを取得します。
	Entity GetEntityID() const;

	ObjectType GetObjectType() const;

	std::string GetName() const;

	template<typename T, typename = EnableIfComponent<T>>
	void AddComponent(const T& component) {
		if (!isComponentAllowedForType<T>()) {
			SystemState::ShowWarning("This component is not allowed for this object type.");
			return; // 処理を取り消す
		}
		componentManager->AddComponent(entity, component);
	}

	// コンポーネントを取得します。
	template<typename T, typename = EnableIfComponent<T>>
	std::optional<std::reference_wrapper<T>> GetComponent() {
		std::optional<std::reference_wrapper<T>> component = componentManager->GetComponent<T>(entity);
		return component;
	}

	// 子オブジェクトを更新します。
	void UpdateChildren(float deltaTime);

	inline void SetName(const std::string& newName) { name = newName; }
	inline void SetEntity(const Entity& id) { entity = id; }
	inline void SetObjectType(const ObjectType& objectType) { type = objectType; }

private:

	// ObjectType に基づいて型チェックを行う
	template<typename T>
	bool isComponentAllowedForType() const {
		if constexpr (std::is_same_v<T, TransformComponent>) {
			return type == ObjectType::Object;
		}
		else if constexpr (std::is_same_v<T, RenderComponent>) {
			return type == ObjectType::Object;
		}
		else if constexpr (std::is_same_v<T, PhysicsComponent>) {
			return type == ObjectType::Object;
		}
		else if constexpr (std::is_same_v<T, MeshComponent>) {
			return type == ObjectType::Object;
		}
		else if constexpr (std::is_same_v<T, CameraComponent>) {
			return type == ObjectType::Camera;
		}
		else if constexpr (std::is_same_v<T, MaterialComponent>) {
			return type == ObjectType::Object;
		}
		else if constexpr (std::is_same_v<T, SpriteComponent>) {
			return type == ObjectType::Sprite;
		}
		else if constexpr (std::is_same_v<T, ParticleComponent>) {
			return type == ObjectType::Particle;
		}
		else if constexpr (std::is_same_v<T, EmitterComponent>) {
			return type == ObjectType::Particle;
		}
		else if constexpr (std::is_same_v<T, AnimationComponent>) {
			return type == ObjectType::Object;
		}
		else if constexpr (std::is_same_v<T, ColliderComponent>) {
			return type == ObjectType::Object;
		}
		else if constexpr (std::is_same_v<T, DirectionalLightComponent>) {
			return type == ObjectType::DirectionalLight;
		}
		else if constexpr (std::is_same_v<T, PointLightComponent>) {
			return type == ObjectType::PointLight;
		}
		else if constexpr (std::is_same_v<T, SpotLightComponent>) {
			return type == ObjectType::SpotLight;
		}
		else if constexpr (std::is_same_v<T, MapChipBlockComponent>) {
			return type == ObjectType::MapChipBlock;
		}
		else if constexpr (std::is_same_v<T, EffectComponent>) {
			return type == ObjectType::Effect;
		}
		// スクリプトは最後にチェック
		else if constexpr (std::is_same_v<T, ScriptComponent>) {
			if (type == ObjectType::Object ||
				type == ObjectType::Camera ||
				type == ObjectType::DirectionalLight ||
				type == ObjectType::SpotLight ||
				type == ObjectType::PointLight ||
				type == ObjectType::Particle ||
				type == ObjectType::Sprite) {
				return true;
			}
			else {
				return false;
			}
		}
		else {
			return false; // デフォルト値を明示的に指定
		}
	}


private:
    Entity entity;
    std::string name = "";
    ObjectType type = ObjectType::Object;
    EntityManager* entityManager=nullptr;
    ComponentManager* componentManager=nullptr;
    std::vector<GameObject*> children;
    GameObject* parent = nullptr;
};
