#pragma once
#include"ConstantData/EmitterData.h"

struct EmitterComponent {

	Vector3 position = { 0.0f,0.0f,0.0f };// 位置
	float radius = 1.0f;// 射出半径
	uint32_t count = 10;// 射出数
	float frequency = 0.5f;// 射出間隔
	float frequencyTime = 0.0f;// 射出間隔調整用時間
	uint32_t emit = 0;// 射出許可

	ConstBufferDataEmitter* constData = nullptr;

	uint32_t cbvIndex = 0;

};