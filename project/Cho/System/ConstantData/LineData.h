#pragma once
#include"Color.h"
#include"ChoMath.h"

//struct LineVertex {
//	Vector3 start;
//	Vector3 end;
//	Color color;
//};
//
//struct LineData {
//	std::vector<LineVertex> lineVertex;
//	LineVertex* mapped = nullptr;
//
//	uint32_t meshViewIndex = 0;
//};

struct LineVertex {
	Vector3 position;
	Color color;
};

struct LineVertexData {
	LineVertex* mapped = nullptr;

	uint32_t meshViewIndex = 0;
};