#pragma once
#include "ChoMath.h"
#include "Core/Utility/Color.h"
// 定数バッファ用データ構造体
// Transform
struct BUFFER_DATA_TF final
{
	// ローカル → ワールド変換行列
	Matrix4 matWorld;		// 64バイト
    Matrix4 worldInverse;	// 64バイト
	// モデルのRootMatrix
    Matrix4 rootMatrix;		// 64バイト
	uint32_t materialID;	// 4バイト
	float padding[3];	// 12バイト
};
// ViewProjection
struct BUFFER_DATA_VIEWPROJECTION final
{
    Matrix4 view;				// 64バイト
    Matrix4 projection;			// 64バイト
    Matrix4 projectionInverse;	// 64バイト
    Matrix4 matWorld;			// 64バイト
    Matrix4 matBillboard;		// 64バイト
	Vector3 cameraPosition;		// 12バイト
	float padding1;				// 4バイト
};
// Line
struct BUFFER_DATA_LINE final
{
	Vector3 position;	// 12バイト
	Color color;		// 16バイト
};
// Material
struct BUFFER_DATA_MATERIAL final
{
	Color color;		// 16バイト
	Matrix4 matUV;		// 64バイト
	float shininess;	// 4バイト
	int enableLighting;	// 4バイト
	int enableTexture;	// 4バイト
	int textureId;		// 4バイト
};
// Emitter
struct BUFFER_DATA_EMITTER final
{
	Vector3 position;	// 12バイト
	float radius;		// 4バイト
	uint32_t count;		// 4バイト
	float frequency;	// 4バイト
	float frequencyTime;// 4バイト
	uint32_t emit;		// 4バイト
};
// Particle
// PerFrame
struct BUFFER_DATA_PARTICLE_PERFRAME final
{
	float time;
	float deltaTime;
	float padding[2];
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
struct ParticlePositionData
{
	uint32_t type;		// 位置のタイプ
	Vector3 value;		// 位置
	PVAData pva;		// PVA
};

struct ParticleRotationData
{
	uint32_t type;		// 位置のタイプ
	Vector3 value;		// 位置
	PVAData pva;		// PVA
};

struct ParticleScaleData
{
	uint32_t type;		// 位置のタイプ
	Vector3 value;		// 位置
	PVAData pva;		// PVA
};
struct ParticlePVAData
{
	Vector3 value;
	Vector3 velocity;
	Vector3 acceleration;
};
struct BUFFER_DATA_PARTICLE final
{
	ParticlePVAData position;	// 36バイト
	ParticlePVAData rotation;	// 36バイト
	ParticlePVAData scale;		// 36バイト
	Color color;				// 16バイト
	float lifeTime;				// 4バイト
	float currentTime;			// 4バイト
	int isAlive;				// 4バイト
};

struct BUFFER_DATA_UISPRITE final
{
	Matrix4 matWorld;	// 64バイト
};