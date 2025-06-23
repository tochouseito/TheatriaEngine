#pragma once
#include <cstdint>
#include <bitset>
using Entity = uint32_t;
using CompID = size_t;
using Archetype = std::bitset<256>;
using PrefabID = uint32_t;
using SceneID = uint32_t;

// オブジェクトのタイプ
enum class ObjectType
{
	MeshObject = 0,		// メッシュオブジェクト
	Camera,				// カメラオブジェクト
	ParticleSystem,		// パーティクルシステムオブジェクト
	Effect,				// エフェクトオブジェクト
	Light,				// ライトオブジェクト
	UI,					// UIオブジェクト
	None,				// オブジェクトなし
	Count,				// カウント
};

// ライトのタイプ
enum class LightType : uint32_t
{
	Directional = 0,	// 平行光源
	Point,				// 点光源
	Spot,				// スポットライト
	Count,
};

inline const char* ObjectTypeToWString(ObjectType type)
{
	switch (type)
	{
	case ObjectType::MeshObject: return "MeshObject";
	case ObjectType::Camera:  return "Camera";
	case ObjectType::ParticleSystem: return "ParticleSystem";
	case ObjectType::Effect:  return "Effect";
	case ObjectType::Light:  return "Light";
	case ObjectType::UI:     return "UI";
	case ObjectType::None:   return "None";
	default:                  return "Unknown";
	}
}

inline ObjectType ObjectTypeFromString(const std::string& str)
{
	if (str == "MeshObject") return ObjectType::MeshObject;
	if (str == "Camera") return ObjectType::Camera;
	if (str == "ParticleSystem") return ObjectType::ParticleSystem;
	if (str == "Effect") return ObjectType::Effect;
	if (str == "Light") return ObjectType::Light;
	if (str == "UI") return ObjectType::UI;
	if (str == "None") return ObjectType::None;
	return ObjectType::Count; // または Unknown があればそちら
}