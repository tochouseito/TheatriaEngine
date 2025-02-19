#pragma once

#include "ECS/ComponentManager/Components/Components.h"
#include "Color.h"
#include "BlendMode.h"
#include "Matrix4.h"
#include "Vector3.h"
#include "SystemState/SystemState.h"
#include "ConstantData/EffectData.h"

// C++
#include <string>
#include <memory>

enum EmitType {
	EmitPoint = 0,		// 点
	EmitCircle,			// 円形
	EmitSphere,			// 球形
};

enum InfluenceType {
	InfluAlways = 0, // 常に影響
	InfluStart,		// 開始時のみ影響
	InfluNone,		// 影響なし
};

enum TriggerType {
	TriggerNone = 0,	// なし
	Trigger1,	// トリガー1
	Trigger2,	// トリガー2
	Trigger3,	// トリガー3
	Trigger4,	// トリガー4
};

enum MaterialType {
	MaterialStandard = 0,	// 標準
};

enum FilterType {
	Nearest = 0,	// 最近傍
	Linear,			// 線形
};

enum FadeOutType {
	FadeOutNone = 0,		// なし
	FadeOutInLifeTime,		// ライフタイム中
	FadeOutDeleted,		// 削除後
};

enum UVType {
	UVStandard = 0,	// 標準
	UVConst,			// 固定
	UVAnimation,		// アニメーション
	UVScroll,			// スクロール
	UVFCurve,			// Fカーブ
};

enum RibbonUVType {
	RibbonUVStretch = 0,	// ストレッチ
	RibbonUVTile,			// タイル
};

enum GetColorTimeType {
	FirstParticle = 0,	// 最初のパーティクル
	Group,				// グループ
};

enum ColorInfluenceType {
	ColorInfluNone = 0,		// 影響なし
	ColorInfluNoneRoot,		// 影響なし(ルート)
	ColorInfluEmit,			// エミット時
	ColorInfluAlways,			// 常に
};

enum EffectMeshType {
	EffectMeshTypeNone = 0,		// なし
	EffectMeshTypeSprite,			// スプライト
	EffectMeshTypeRibbon,			// リボン
	EffectMeshTypeTrajectory,		// 軌跡
	EffectMeshTypeRing,			// リング
	EffectMeshTypeModel,			// モデル
};

enum RingType {
	Doughnut = 0,	// ドーナツ
	Crescent,		// 三日月
};

enum ColorType {
	ColorConst = 0,		// 固定
	ColorRandom,			// ランダム
	ColorEasing,			// イージング
	ColorFCurve,			// Fカーブ
	ColorGradient,		// グラデーション
};

enum DrawingOrder {
	DrawOrderNormal = 0,		// 通常
	DrawOrderInverse,		// 逆
};

enum Placement {
	PlacementBillboard = 0,	  // ビルボード
	PlacementZAxisRotBillboard,// Z軸回転ビルボード
	PlacementYAxisConst,		  // Y軸固定
	PlacementConst,			  // 固定
};

enum VertexColorType {
	VertexColorStandard = 0,	// 標準
	VertexColorConst,			// 固定
};

enum VertexPositionType {
	VertexPositionStandard = 0,	// 標準
	VertexPositionConst,			// 固定
};

enum PositionType {
	PositionStandard = 0,	// 標準
	PositionPVA,					// PVA
	PositionEasing,			// イージング
	PositionFCurve,			// Fカーブ
	PositionNURBS,			// NURBS
	PositionCameraOffset,	// カメラオフセット
};

enum RotateType {
	RotateStandard = 0,	// 標準
	RotatePVA,				// PVA
	RotateEasing,		// イージング
	RotateFCurve,		// Fカーブ
	RotateNURBS,		// NURBS
	RotateCameraOffset,	// カメラオフセット
};

enum ScaleType {
	ScaleStandard = 0,	// 標準
	ScalePVA,				// PVA
	ScaleEasing,		// イージング
	ScaleFCurve,		// Fカーブ
	ScaleNURBS,			// NURBS
	ScaleCameraOffset,	// カメラオフセット
};

enum TextureAddressMode {
	Repeat = 0,		// 繰り返し
	Clamp,			// クランプ
};

struct RandValue// ランダム値
{
	float first = 0.0f;		    // 最大,中央値
	float second = 0.0f;        // 最小,広がり幅
};

struct LifeTime// ライフタイム
{
	float lifeTime = 0.0f;      // ライフタイム
	//float currentTime = 0.0f;   // 現在の時間
	// Random値形式
	bool isMedian = true;      // ランダム値の設定方法。最大・最小、中央値・広がり幅の2種類
	RandValue randValue;	    // ランダム値
};

struct EmitTime// エミット時間
{
	float emitTime = 0.0f;      // エミット時間
	//float currentTime = 0.0f;   // 現在の時間
	// Random値形式
	bool isMedian = true;	    // ランダム値の設定方法。最大・最小、中央値・広がり幅の2種類
	RandValue randValue;	    // ランダム値
};

struct EmitStartTime// エミット開始時間
{
	float emitStartTime = 0.0f; // エミット開始時間
	//float currentTime = 0.0f;	// 現在の時間
	// Random値形式
	bool isMedian = true;		// ランダム値の設定方法。最大・最小、中央値・広がり幅の2種類
	RandValue randValue;		// ランダム値
};

struct Radius// 半径
{
	float radius = 0.0f;		// 半径
	// Random値形式
	bool isMedian = true;	// ランダム値の設定方法。最大・最小、中央値・広がり幅の2種類
	RandValue randValue;	// ランダム値
};

struct StartAngle// 開始角度
{
	float startAngle = 0.0f;	// 開始角度
	// Random値形式
	bool isMedian = true;	// ランダム値の設定方法。最大・最小、中央値・広がり幅の2種類
	RandValue randValue;	// ランダム値
};

struct EndAngle// 終了角度
{
	float endAngle = 0.0f;		// 終了角度
	// Random値形式
	bool isMedian = true;	// ランダム値の設定方法。最大・最小、中央値・広がり幅の2種類
	RandValue randValue;	// ランダム値
};

struct CircleEmitPosition {
	bool isInfluenceAngle = false;	// 角度に影響を受けるか
	Radius radius;					// 半径
	StartAngle startAngle;			// 開始角度
	EndAngle endAngle;				// 終了角度
	uint32_t divisions = 8;			// 分割数
};

struct Angle// 角度
{
	float angle = 0.0f;	// 角度
	// Random値形式
	bool isMedian = true;// ランダム値の設定方法。最大・最小、中央値・広がり幅の2種類
	RandValue randValue;	// ランダム値
};

struct XAxisAngle// 軸角度
{
	Vector3 axis = Vector3(1.0f, 0.0f, 0.0f);	// 軸
	Angle angle;								// 角度
};

struct YAxisAngle// 軸角度
{
	Vector3 axis = Vector3(0.0f, 1.0f, 0.0f);	// 軸
	Angle angle;								// 角度
};

struct SphereEmitPosition {
	bool isInfluenceAngle = false;	// 角度に影響を受けるか
	Radius radius;					// 半径
	XAxisAngle xAxisAngle;			// X軸角度
	YAxisAngle yAxisAngle;			// Y軸角度
};

struct PointEmitPosition {
	bool isInfluenceAngle = false;	// 角度に影響を受けるか
	bool isMedian = true;			// ランダム値の設定方法。最大・最小、中央値・広がり幅の2種類
	RandValue randValue;			// ランダム値
};

struct EffectVector3// 位置,角度,拡縮
{
	Vector3 first = Vector3(0.0f, 0.0f, 0.0f);	// 最大,中央値
	Vector3 second = Vector3(0.0f, 0.0f, 0.0f);	// 最小,広がり幅
	// Random値形式
	bool isMedian = true;							// ランダム値の設定方法。最大・最小、中央値・広がり幅の2種類
};

struct EffectVelocity// 速度
{
	Vector3 first = Vector3(0.0f, 0.0f, 0.0f);	// 最大,中央値
	Vector3 second = Vector3(0.0f, 0.0f, 0.0f);	// 最小,広がり幅
	// Random値形式
	bool isMedian = true;							// ランダム値の設定方法。最大・最小、中央値・広がり幅の2種類
};

struct EffectAcceleration// 加速度
{
	Vector3 first = Vector3(0.0f, 0.0f, 0.0f);	// 最大,中央値
	Vector3 second = Vector3(0.0f, 0.0f, 0.0f);	// 最小,広がり幅
	// Random値形式
	bool isMedian = true;							// ランダム値の設定方法。最大・最小、中央値・広がり幅の2種類
};

struct PVA // Position,Velocity,Acceleration
{
	EffectVector3 value;			// 位置
	EffectVelocity velocity;		// 速度
	EffectAcceleration acceleration;// 加速度
};

struct EffectPosition// 位置
{
	uint32_t type = PositionStandard;				// 位置タイプ

	Vector3 value;								// 位置
	PVA pva;
};

struct EffectRotate// 回転
{
	uint32_t type = RotateStandard;				// 回転タイプ

	Vector3 value;								// 回転
	PVA pva;
};

struct EffectScale// スケール
{
	uint32_t type = ScaleStandard;				// スケールタイプ

	Vector3 value = { 1.0f,1.0f,1.0f };								// スケール
	PVA pva;
};

struct EffectNodeCommon {
	bool visible = true;				  // 表示・非表示
	std::string name = "Node";				  // ノード名
	uint32_t maxCount = 256;				  // 生成数
	uint32_t emitCount = 1;				  // エミット数
	uint32_t PosInfluenceType = InfluAlways;   // 位置の親による子の影響
	uint32_t RotInfluenceType = InfluAlways;   // 回転の親による子の影響
	uint32_t SclInfluenceType = InfluAlways;   // スケールの親による子の影響
	bool deleteLifetime = false;		  // ライフタイムが終了したら削除する
	bool deleteParentDeleted = false;	  // 親が削除されたら削除する
	bool deleteAllChildrenDeleted = false;// 子が全て削除されたら削除する
	LifeTime lifeTime;					  // ライフタイム
	EmitTime emitTime;					  // エミット時間
	EmitStartTime emitStartTime;		  // エミット開始時間
	uint32_t emitStartTrigger = TriggerNone;	  // エミット開始トリガー
	uint32_t emitStopTrigger = TriggerNone;	  // エミット停止トリガー
	uint32_t deleteTrigger = TriggerNone;		  // 削除トリガー
};

struct EffectDrawCommon {
	uint32_t materialType = MaterialStandard;  // マテリアルタイプ
	float emissiveMagnification = 1.0f;// エミッシブ倍率
	std::string textureName = "";	   // テクスチャ名
	uint32_t filterType = Linear;	   // フィルタータイプ
	uint32_t addressMode = Repeat;	   // アドレスモード
	uint32_t blendMode = kBlendModeNormal;	   // ブレンドモード
	bool DepthWrite = false;		   // 深度書き込み
	bool DepthTest = true;			   // 深度テスト
	bool isFadeIn = false;			   // フェードイン
	uint32_t fadeOutType = FadeOutNone;	   // フェードアウトタイプ
	uint32_t uvType = UVStandard;		   // UVタイプ
	uint32_t colorInfluenceType = ColorInfluNone;// カラー影響タイプ
};

struct EffectMeshSprite {
	uint32_t colorType = ColorConst;				// カラータイプ
	Color color;					// カラー
	uint32_t drawingOrder = DrawOrderNormal;			// 描画順
	uint32_t placement = PlacementBillboard;			// 配置
	uint32_t vertexColorType = VertexColorStandard;	// 頂点カラータイプ
	uint32_t vertexPositionType = VertexPositionStandard; // 頂点位置タイプ
};

struct EffectMeshRibbon {
	uint32_t colorType = ColorConst;				// カラータイプ
	uint32_t uvType = RibbonUVStretch;	// UVタイプ
	uint32_t getColorType = Group;		// カラー取得タイプ
	bool isViewTracking = false;		// 視線追従
	Color color;					// カラー
	uint32_t vertexColorType = VertexColorStandard;	// 頂点カラータイプ
	uint32_t positionType = VertexPositionStandard;		// 位置タイプ
	uint32_t splineDivision = 1;		// スプライン分割数
};

struct EffectMeshTrajectory {
	uint32_t uvType = RibbonUVStretch;	// UVタイプ
	bool isSmooth = true;				// スムーズ
};

struct EffectMeshRing {
	uint32_t ringType = Doughnut;		// リングタイプ
};

struct EffectMeshModel {
	//std::string modelName = "";		// モデル名
	uint32_t meshIndex = 0;			// メッシュインデックス
};

struct EffectDraw {
	uint32_t meshType = EffectMeshTypeNone;				// メッシュタイプ
	EffectMeshSprite meshSprite;		// スプライト
	EffectMeshRibbon meshRibbon;		// リボン
	EffectMeshTrajectory meshTrajectory;	// 軌跡
	EffectMeshRing meshRing;			// リング
	EffectMeshModel meshModel;			// モデル
};

struct TimeManager {
	float globalTime = 0.0f;  // 全体の経過時間
	float maxTime = 120.0f;	   // 最大時間
	float deltaTime = 0.0f;	   // デルタ時間
	float preTime = 0.0f;	   // 前回時間
};

struct EffectParticle {
	uint32_t particleUAVIndex = 0;	// パーティクルインデックス
	FreeListIndex freeListIndex;			// フリーリストインデックス
	FreeList freeList;						// フリーリスト
};

struct EffectNode {
	EffectNodeCommon common;				// 共通
	EffectPosition position;				// 位置
	EffectRotate rotation;					// 回転
	EffectScale scale;						// スケール
	EffectDrawCommon drawCommon;			// 描画共通
	EffectDraw draw;					    // 描画
	EffectParticle particle;				// パーティクル

	uint32_t srvIndex = 0;					// SRVIndex
	uint32_t vertexUAVIndex = 0;			// VertexUAVIndex
	uint32_t indexUAVIndex = 0;				// IndexUAVIndex

	std::vector<EffectNode> children;		// 子ノード
	EffectNode* parent = nullptr;		// 親ノード

	ConstBufferDataEffectNode* constData = nullptr;			// インスタンスデータ
	// 後で使うuint32_t instanceIndex = 0;				// インスタンスインデックス

	// Ex
	uint32_t EmitType = EmitPoint;				// エミットタイプ
	PointEmitPosition pointEmitPosition;	// 点エミット位置
	CircleEmitPosition circleEmitPosition;  // 円形エミット位置
	SphereEmitPosition sphereEmitPosition;  // 球形エミット位置
};

struct EffectComponent
{
	uint32_t numInstance = 1;	// 描画するインスタンス数
	std::vector<EffectNode> effectNodes;	// エミットノード

	/* 後で使う
	uint32_t srvIndex = 0;		// SRVIndex
	uint32_t instanceCount = 0;	// インスタンス数
	ConstBufferDataEffectNode* constData = nullptr;	// インスタンスデータ
	// 後で使う*/

	bool isInit = false;	// 初期化フラグ
	bool isRun = false;	// 実行フラグ
	bool isLoop = false;	// ループフラグ
	bool isEnd = true;	// 終了フラグ

	TimeManager* timeManager=nullptr;	// タイムマネージャ
	uint32_t timeManagerCBVIndex = 0;	// TimeManagerCBVIndex

	// コンポーネント
	RenderComponent render;
	MeshComponent mesh;
	MaterialComponent material;
};