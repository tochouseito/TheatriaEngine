#pragma once

// Mathライブラリ
#include "ChoMath.h"

// C++
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>
#include <typeindex>
#include <typeinfo>
#include <cassert>
#include <type_traits>

// Data
#include "Generator/MeshGenerator/MeshGenerator.h"

// Components
#include "ECS/ComponentManager/Components/CameraComponent/CameraComponent.h"
#include "ECS/ComponentManager/Components/MaterialComponent/MaterialComponent.h"
#include "ECS/ComponentManager/Components/MeshComponent/MeshComponenth.h"
#include "ECS/ComponentManager/Components/PhysicsComponent/PhysicsComponent.h"
#include "ECS/ComponentManager/Components/RenderComponent/RenderComponent.h"
#include "ECS/ComponentManager/Components/TransformComponent/TransformComponent.h"
#include "ECS/ComponentManager/Components/ScriptComponent/ScriptComponent.h"
#include "ECS/ComponentManager/Components/SpriteComponent/SpriteComponent.h"
#include "ECS/ComponentManager/Components/ParticleComponent/ParticleComponent.h"
#include "ECS/ComponentManager/Components/EmitterComponent/EmitterComponent.h"
#include "ECS/ComponentManager/Components/AnimationComponent/AnimationComponent.h"
#include "ECS/ComponentManager/Components/ColliderComponent/ColliderComponent.h"
#include "ECS/ComponentManager/Components/DirectionalLightComponent/DirectionalLightComponent.h"
#include "ECS/ComponentManager/Components/PointLightComponent/PointLightComponent.h"
#include "ECS/ComponentManager/Components/SpotLightComponent/SpotLightComponent.h"
#include "ECS/ComponentManager/Components/MapChipBlockComponent/MapChipBlockComponent.h"
#include "ECS/ComponentManager/Components/EffectComponent/EffectComponent.h"

#include "ObjectType/ObjectType.h"

// 有効な型リスト
template<typename T>
using EnableIfComponent=std::enable_if_t<
	std::is_same_v<T, TransformComponent> ||
	std::is_same_v<T, RenderComponent> ||
	std::is_same_v<T, PhysicsComponent> ||
	std::is_same_v<T, MeshComponent> ||
	std::is_same_v<T, CameraComponent> ||
	std::is_same_v<T, MaterialComponent> ||
	std::is_same_v<T, ScriptComponent> ||
	std::is_same_v<T, SpriteComponent> ||
	std::is_same_v<T, ParticleComponent> ||
	std::is_same_v<T, EmitterComponent> ||
	std::is_same_v<T, AnimationComponent> ||
	std::is_same_v<T, ColliderComponent> ||
	std::is_same_v<T, DirectionalLightComponent> ||
	std::is_same_v<T, PointLightComponent> ||
	std::is_same_v<T, SpotLightComponent> ||
	std::is_same_v<T, MapChipBlockComponent>||
	std::is_same_v<T, EffectComponent>
>;