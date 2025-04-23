#pragma once
#include <cstdint>
#include <bitset>
using Entity = uint32_t;
using CompID = size_t;
using Archetype = std::bitset<256>;
using ObjectID = uint32_t;
using PrefabID = uint32_t;

// オブジェクトのタイプ
enum class ObjectType
{
	MeshObject = 0,		// メッシュオブジェクト
	Camera,				// カメラオブジェクト
	ParticleSystem,		// パーティクルシステムオブジェクト
	Effect,				// エフェクトオブジェクト
	//Light,			// ライトオブジェクト
	Count,				// カウント
};

inline const char* ObjectTypeToWString(ObjectType type)
{
	switch (type)
	{
	case ObjectType::MeshObject: return "MeshObject";
	case ObjectType::Camera:  return "Camera";
	case ObjectType::ParticleSystem: return "ParticleSystem";
	case ObjectType::Effect:  return "Effect";
	default:                  return "Unknown";
	}
}

inline ObjectType ObjectTypeFromString(const std::string& str)
{
	if (str == "MeshObject") return ObjectType::MeshObject;
	if (str == "Camera") return ObjectType::Camera;
	if (str == "ParticleSystem") return ObjectType::ParticleSystem;
	if (str == "Effect") return ObjectType::Effect;
	return ObjectType::Count; // または Unknown があればそちら
}