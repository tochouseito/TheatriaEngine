#pragma once
#include<cstdint>
// Mesh
enum class MeshPattern :uint32_t
{
	Cube,
	Plane,
	Sphere,
	CountPattern,// カウント用
};