#pragma once
#include"ChoMath.h"
struct State {
	uint32_t state = 0;
};

struct AnimationComponent
{
	float time = 0.0f;// 現在のアニメーションの時間
	float transitionTime = 0.0f;// 遷移中経過時間
	float transitionDuration = 0.1f;// 遷移にかける時間
	float lerpTime = 0.0f;// 線形補間の時間
	bool transition = false;
	bool isEnd = false;// ループするかどうか
	bool isRun = true;// アニメーションを再生するかどうか
	bool isRestart = true;// アニメーションを再生するかどうか
	//std::string modelName = "";
	uint32_t numAnimation = 0;// アニメーションの数
	uint32_t animationIndex = 0;// アニメーションのIndex
	uint32_t prevAnimationIndex = 0;// 1つ前のアニメーションのIndex
	uint32_t transitionIndex = 0;// 遷移スタートのアニメーションのIndex
	
	uint32_t nowFrame = 0;// 現在のフレーム
	uint32_t allFrame = 0;// 全フレーム数
};