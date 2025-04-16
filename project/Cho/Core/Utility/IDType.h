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