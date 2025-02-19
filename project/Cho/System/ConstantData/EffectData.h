#pragma once

#include "ChoMath.h"
#include "Color.h"

struct EffectTime {
	float globalTime;             // 経過時間
};

struct RandValueData// ランダム値
{
	float first = 0.0f;		    // 最大,中央値
	float second = 0.0f;        // 最小,広がり幅
};

struct LifeTimeData// ライフタイム
{
	float lifeTime = 0.0f;      // ライフタイム
	// Random値形式
	uint32_t isMedian;      // ランダム値の設定方法。最大・最小、中央値・広がり幅の2種類
	RandValueData randValue;	    // ランダム値
};

struct EmitTimeData// エミット時間
{
	float emitTime = 0.0f;      // エミット時間
	// Random値形式
	uint32_t isMedian;	    // ランダム値の設定方法。最大・最小、中央値・広がり幅の2種類
	RandValueData randValue;	    // ランダム値
};

struct EmitStartTimeData// エミット開始時間
{
	float emitStartTime = 0.0f; // エミット開始時間
	// Random値形式
	uint32_t isMedian;		// ランダム値の設定方法。最大・最小、中央値・広がり幅の2種類
	RandValueData randValue;		// ランダム値
};

struct EffectCommonData {
	uint32_t maxCount = 256;				  // 生成数
	uint32_t emitCount = 1;				  // 1フレームあたりの生成数
	uint32_t PosInfluenceType;   // 位置の親による子の影響
	uint32_t RotInfluenceType;   // 回転の親による子の影響
	uint32_t SclInfluenceType;   // スケールの親による子の影響
	uint32_t deleteLifetime;		  // ライフタイムが終了したら削除する
	uint32_t deleteParentDeleted;	  // 親が削除されたら削除する
	uint32_t deleteAllChildrenDeleted;// 子が全て削除されたら削除する
	LifeTimeData lifeTime;				  // ライフタイム
	EmitTimeData emitTime;				  // エミット時間
	EmitStartTimeData emitStartTime;	  // エミット開始時間
};

struct PVAVector3Data// 位置,角度,拡縮
{
	Vector3 first = Vector3(0.0f, 0.0f, 0.0f);	// 最大,中央値
	Vector3 second = Vector3(0.0f, 0.0f, 0.0f);	// 最小,広がり幅
	// Random値形式
	uint32_t isMedian;							// ランダム値の設定方法。最大・最小、中央値・広がり幅の2種類
};

struct PVAData // Position,Velocity,Acceleration
{
	PVAVector3Data value;			// 位置
	PVAVector3Data velocity;		// 速度
	PVAVector3Data acceleration;// 加速度
};

struct EffectPositionData {
	uint32_t type;		// 位置のタイプ
	Vector3 value;		// 位置
	PVAData pva;		// PVA
};

struct EffectRotationData {
	uint32_t type;		// 位置のタイプ
	Vector3 value;		// 位置
	PVAData pva;		// PVA
};

struct EffectScaleData {
	uint32_t type;		// 位置のタイプ
	Vector3 value;		// 位置
	PVAData pva;		// PVA
};

struct EffectDrawCommonData {
	float emissive;		// エミッシブ
	uint32_t fadeOutType;	// フェードアウトタイプ
};

struct EffectMeshSpriteData {
	Color color;		// 色
	uint32_t placement;	// 配置
};

struct EffectDrawData {
	uint32_t meshType;	// メッシュタイプ
	EffectMeshSpriteData meshSprite;	// メッシュスプライト
};

struct ConstBufferDataEffectNode {
	EffectCommonData common;					// 共通
	EffectPositionData position;				// 位置
	EffectRotationData rotation;				// 回転
	EffectScaleData scale;						// スケール
	EffectDrawCommonData drawCommon;			// 描画共通
	EffectDrawData draw;					    // 描画
	uint32_t parentIndex;						// 親のインデックス
};