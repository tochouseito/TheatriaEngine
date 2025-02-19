#pragma once

#include"ConstantData/ParticleData.h"
#include"ChoMath.h"
#include"ECS/ComponentManager/Components/MaterialComponent/MaterialComponent.h"
#include"ECS/ComponentManager/Components/RenderComponent/RenderComponent.h"

static const uint32_t kMaxParticle = 1024;// 最大パーティクル

struct PerFrame {
	float time=0.0f;
	float deltaTime=0.0f;

	ConstBufferDataPerFrame* constData = nullptr;

	uint32_t cbvIndex = 0;

};

struct Counter {
	uint32_t uavIndex = 0;
};
struct FreeListIndex {
	int32_t uavIndex = 0;
};
struct FreeList {
	uint32_t uavIndex = 0;
};

struct ParticleComponent {

	uint32_t count = 512;// パーティクル数

	Matrix4 matBillboard = ChoMath::MakeIdentity4x4();

	PerFrame perFrame;
	Counter counter;
	FreeListIndex freeListIndex;
	FreeList freeList;

	MaterialComponent material;
	RenderComponent render;

	//ConstBufferDataParticle* constData = nullptr;

	uint32_t uavIndex = 0;
};