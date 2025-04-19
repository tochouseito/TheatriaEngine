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
	MeshObject = 0,// メッシュオブジェクト
	Camera,// カメラオブジェクト
	Count,// カウント
};

inline const char* ObjectTypeToWString(ObjectType type)
{
	switch (type)
	{
	case ObjectType::MeshObject: return "MeshObject";
	case ObjectType::Camera:  return "Camera";
	default:                  return "Unknown";
	}
}

inline ObjectType ObjectTypeFromString(const std::string& str)
{
	if (str == "MeshObject") return ObjectType::MeshObject;
	if (str == "Camera") return ObjectType::Camera;
	return ObjectType::Count; // または Unknown があればそちら
}