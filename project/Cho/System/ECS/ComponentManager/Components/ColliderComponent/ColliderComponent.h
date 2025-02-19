#pragma once

#include"ChoMath.h"

enum ColliderType {
	Box,
	Sphere,
	Capsule,
	Mesh,
};

struct ColliderComponent {
	Vector3 center = { 0.0f, 0.0f, 0.0f };
	float radius = 1.0f;
	bool visible = false;
	ColliderType type = Sphere;
};