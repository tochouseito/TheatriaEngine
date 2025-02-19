#pragma once
#include"ConstantData/SpriteData.h"
#include<string>
#include"Base/Color.h"
#include"BlendMode.h"
#include"ChoMath.h"

#include"ECS/ComponentManager/Components/MaterialComponent/MaterialComponent.h"

// sprite
struct SpriteComponent final {
	Vector2 position{ 0.0f,0.0f };// 位置
	float rotation = 0.0f;// Z軸回転
	Vector2 scale = { 1.0f,1.0f };// スケール

	Matrix4 matWorld = ChoMath::MakeIdentity4x4();

	Vector2 uvPos = { 0.0f,0.0f };
	float uvRot = 0.0f;
	Vector2 uvScale = { 1.0f,1.0f };

	MaterialComponent material;
	RenderComponent render;

	Vector2 anchorPoint = { 0.0f,0.0f };// アンカーポイント
	Vector2 size = { 64.0f,64.0f };// サイズ
	Vector2 textureLeftTop = { 0.0f,0.0f };// テクスチャの左上座標
	Vector2 textureSize = { 64.0f,64.0f };// テクスチャのサイズ

	uint32_t spriteIndex = 0;

	BlendMode blendMode = BlendMode::kBlendModeNone;// ブレンドモード

	ConstBufferDataSprite* constData = nullptr;

	uint32_t cbvIndex = 0;
};