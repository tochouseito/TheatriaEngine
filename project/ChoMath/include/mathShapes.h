#pragma once
#include"Vector3.h"
struct Sphere {
	Vector3 center; // !< 中心点
	float radius;   // !< 半径
};
struct Line {
	Vector3 origin; // !<始点
	Vector3 diff;   // !<終点への差分ベクトル
};
struct Ray {
	Vector3 origin; // !<始点
	Vector3 diff;   // !<終点への差分ベクトル
};
struct Segment {
	Vector3 origin; // !<始点
	Vector3 diff;   // !<終点への差分ベクトル
};
struct Plane {
	Vector3 normal; //!< 法線
	float distance; //!< 距離
};
struct Triangle
{
	Vector3 vertices[3];//!< 頂点
};
struct AABB {
	Vector3 min; //!<最小点
	Vector3 max; //!<最大点
};
struct Vector2Int {
	int x;
	int y;
};
struct OBB {
	Vector3 center; //!<中心点
	Vector3 orientations[3]; //!<座標軸、正規化，直交必須
	Vector3 size; //!< 座標方向の長さの半分。中心から面までの距離
};
