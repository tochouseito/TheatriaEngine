#pragma once
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4.h"
#include "Core/Utility/Color.h"
#include <cstdint>
#include <vector>

struct RandValue
{
    float median;       // 中央値
    float amplitude;    // 振幅
};

struct RandVector3
{
    RandValue x;
    RandValue y;
    RandValue z;
};

// イージングタイプ
enum class EFFECT_EASING_TYPE
{
    LEFTANDRIGHT = 0,
	LINEAR,
	EASE_IN_QUADRATIC,
	EASE_OUT_QUADRATIC,
	EASE_IN_OUT_QUADRATIC,
	EASE_IN_CUBIC,
	EASE_OUT_CUBIC,
	EASE_IN_OUT_CUBIC,
	EASE_IN_QUARTIC,
	EASE_OUT_QUARTIC,
	EASE_IN_OUT_QUARTIC,
	EASE_IN_QUINTIC,
	EASE_OUT_QUINTIC,
	EASE_IN_OUT_QUINTIC,
	EASE_IN_BACK,
	EASE_OUT_BACK,
	EASE_IN_OUT_BACK,
	EASE_IN_BOUNCE,
	EASE_OUT_BOUNCE,
	EASE_IN_OUT_BOUNCE,
	EASING_TYPE_COUNT
};

// イージングの速度タイプ
enum class EFFECT_SPEED_TYPE
{
	CONSTANT = 0,
	SLOW0,
	FAST0,
	SLOW1,
	FAST1,
	SLOW2,
	FAST2,
	EASING_SPEED_TYPE_COUNT
};

struct EasingValue
{
    RandVector3 startPoint;     // 始点
    RandVector3 endPoint;       // 終点
    uint32_t easingType;            // イージングタイプ
    uint32_t startSpeedType;        // 始点速度タイプ
    uint32_t endSpeedType;          // 終点速度タイプ
    uint32_t isMedianPoint;         // 中間点有効
    RandVector3 medianPoint;    // 中間点
};

struct PVA
{
    RandVector3 value;          // 値
    RandVector3 velocity;       // 速度
    RandVector3 acceleration;   // 加速度
};

enum class EFFECT_SRT_TYPE
{
	SRT_TYPE_STANDARD = 0,
	SRT_TYPE_PVA,
	SRT_TYPE_EASING,
};

struct EffectSRT
{
    uint32_t type;          // タイプ
    Vector3 value;       // 値
    PVA pva;            // 値、速度、加速度
    EasingValue easing; // イージング値
};

enum class INFLUNCE_TYPE
{
	NONE = 0,       // 影響なし
	ALWAYS,   // 常に影響する
};

struct EffectCommon
{
    uint32_t emitCount;                 // 生成数
    uint32_t isUnlimit;                 // 無限生成フラグ
    uint32_t PosInfluenceType;          // 位置影響タイプ
    uint32_t RotInfluenceType;          // 回転影響タイプ
    uint32_t SclInfluenceType;          // スケール影響タイプ
    uint32_t deleteLifetime;            // 寿命で削除
    uint32_t deleteParentDeleted;       // 親削除で削除
    uint32_t deleteAllChildrenDeleted;  // 全ての子削除で削除
    RandValue lifeTime;             // 生存時間
    RandValue emitTime;             // 生成時間
    RandValue emitStartTime;        // 生成開始時間
};

// FadeOutタイプ
enum class FADEOUT_TYPE
{
	NONE = 0,   // フェードアウトしない
	ONLIFETIME, // 生存時間内でフェードアウト
	DELETED,    // 削除後にフェードアウト
	FADEOUT_TYPE_COUNT
};

// UVタイプ
enum class UV_TYPE
{
	STANDARD = 0,   // 標準UV
	CONSTANT,       // 固定UV
	ANIMATION,      // アニメーションUV
	SCROLL,         // スクロールUV
	UV_TYPE_COUNT
};

struct UVConstantValue
{
    Vector2 startPoint;  // 開始UV
    Vector2 scale;       // スケール
};

struct UVAnimationParameter
{
    Vector2 startPoint;      // 開始UV
    Vector2 scale;           // スケール
    uint32_t oneTime;           // 1回分の時間
    uint32_t widthSheetCount;   // 横の枚数
    uint32_t heightSheetCount;  // 縦の枚数
    uint32_t isLoop;            // ループフラグ
    RandValue startSheet;   // 開始シート
    uint32_t interpolation;     // 補間フラグ
};

struct EffectDrawCommon
{
    uint32_t materialType = 0;                      // マテリアルタイプ(現在は通常のみ)
	uint32_t textureID;                          // テクスチャインデックス
    float emissive;                             // 発光値
    uint32_t isFadeIn;                              // フェードイン(0:しない 1:する)
    uint32_t isFadeOutType;                         // フェードアウトタイプ
    UVConstantValue constantValue;              // UV定数値
    UVAnimationParameter animationParameter;    // UVアニメーションパラメータ
};

enum class EFFECT_MESH_TYPE
{
	NONE = 0,       // メッシュタイプなし
	SPRITE,         // スプライト
	RIBBON,         // リボン
	TRAIL,          // トレイル
	RING,           // リング
	MODEL,          // モデル
	CUBE,           // 立方体
	SPHERE,         // 球
};

enum class COLOR_TYPE
{
	NONE = 0,       // 色タイプなし
	CONSTANT,       // 固定色
	RANDOM,         // ランダム色
	EASING,         // イージング色
};

struct RandColor
{
    RandValue r; // 赤
    RandValue g; // 緑
    RandValue b; // 青
    RandValue a; // アルファ
};

struct EasingColor
{
    Color startMinColor;   // 始点最小色
    Color startMaxColor;   // 始点最大色
    Color endMinColor;     // 終点最小色
    Color endMaxColor;     // 終点最大色
    EFFECT_SPEED_TYPE startSpeedType;    // 始点速度タイプ
    EFFECT_SPEED_TYPE endSpeedType;      // 終点速度タイプ
};

enum class PLACEMENT_TYPE
{
	NONE = 0,       // 配置タイプなし
	BILLBOARD,      // ビルボード
	BILLBOARDY,     // Y軸ビルボード
	BILLBOARDXY,    // Z軸回転ビルボード
	CONSTANT,       // 固定
};

enum class VERTEX_COLOR_TYPE
{
    STANDARD = 0,       // 標準
    CONSTANT,           // 固定
};

enum class VERTEX_POSITION_TYPE
{
	STANDARD = 0,       // 標準
	CONSTANT,           // 固定
};

struct SpriteVertexColor
{
    Color leftBottom;  // 左下色
    Color rightBottom; // 右下色
    Color leftTop;     // 左上色
    Color rightTop;    // 右上色
};

struct SpriteVertexPosition
{
    Vector2 leftBottom;  // 左下座標
    Vector2 rightBottom; // 右下座標
    Vector2 leftTop;     // 左上座標
    Vector2 rightTop;    // 右上座標
};

struct EffectSprite
{
    COLOR_TYPE ColorType;                         // 色タイプ
    Color color;                           // 色
    RandColor randColor;                    // ランダム色
    EasingColor easingColor;                // イージング色
    PLACEMENT_TYPE placement;                         // 配置方法
    VERTEX_COLOR_TYPE VertexColorType;                   // 頂点色タイプ
    SpriteVertexColor vertexColor;          // 頂点色
	VERTEX_POSITION_TYPE VertexPositionType;                 // 頂点座標タイプ
    SpriteVertexPosition vertexPosition;    // 頂点座標
};

struct EffectDraw
{
	uint32_t meshType;          // メッシュタイプ
	uint32_t meshDataIndex;     // メッシュデータインデックス
};

// ノード
struct EffectNode
{
    EffectCommon common;            // 共通情報
    EffectSRT position;             // 位置
    EffectSRT rotation;             // 回転
    EffectSRT scale;                // スケール
    EffectDrawCommon drawCommon;    // 描画共通情報
    EffectDraw draw;                // 描画情報
    uint32_t parentIndex;           // 親インデックス
};

struct EffectNodeData
{
    std::string name;               // ノード名
    EffectCommon common;            // 共通情報
    EffectSRT position;             // 位置
    EffectSRT rotation;             // 回転
    EffectSRT scale;                // スケール
    EffectDrawCommon drawCommon;    // 描画共通情報
    EffectDraw draw;                // 描画情報
    uint32_t parentIndex;           // 親インデックス
	uint32_t id;                // ノードID
};

struct TimeManager
{
    float globalTime;
    float maxTime;
    float deltaTime;
    float padding[1];
};

struct EffectRoot
{
    TimeManager timeManager;    // 時間管理
    uint32_t isRun;
    uint32_t isLoop;               // ループフラグ
	float padding[2];
	uint32_t nodeID[128];           // ノードID
};

struct EffectData
{
    std::string name;           // エフェクト名
    TimeManager timeManager;    // 時間管理
    uint32_t isRun;
    uint32_t isLoop;               // ループフラグ
	std::vector<uint32_t> nodeID;           // ノードID
};

struct EffectParticlePVA
{
    Vector3 value;
	Vector3 velocity;
	Vector3 acceleration;
};
struct EffectParticle
{
	EffectParticlePVA position;         // 位置
	EffectParticlePVA rotation;         // 回転
	EffectParticlePVA scale;            // スケール
	Color color;               // 色
	float lifeTime;             // 寿命
	float currentTime;         // 現在時間
	uint32_t isAlive;            // 生存フラグ
    uint32_t rootID;
	uint32_t nodeID;
    uint32_t meshID;
};