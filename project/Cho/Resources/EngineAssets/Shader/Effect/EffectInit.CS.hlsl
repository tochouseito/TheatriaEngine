
struct VertexData
{
    float4 position : POSITION;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
    float4 color : COLOR0;
};

struct RandValue
{
    // 中央値
    float median;
    // 振幅
    float amplitude;
};

struct RandVector3
{
    RandValue x;
    RandValue y;
    RandValue z;
};

// イージングタイプ
static const uint EASING_TYPE_COUNT = 20;
static const uint EASING_TYPE_LEFTANDRIGHT = 0;
static const uint EASING_TYPE_LINEAR = 1;
static const uint EASING_TYPE_EASEINQUADRATIC = 2;
static const uint EASING_TYPE_EASEOUTQUADRATIC = 3;
static const uint EASING_TYPE_EASEINOUTQUADRATIC = 4;
static const uint EASING_TYPE_EASEINCUBIC = 5;
static const uint EASING_TYPE_EASEOUTCUBIC = 6;
static const uint EASING_TYPE_EASEINOUTCUBIC = 7;
static const uint EASING_TYPE_EASEINQUARTIC = 8;
static const uint EASING_TYPE_EASEOUTQUARTIC = 9;
static const uint EASING_TYPE_EASEINOUTQUARTIC = 10;
static const uint EASING_TYPE_EASEINQUINTIC = 11;
static const uint EASING_TYPE_EASEOUTQUINTIC = 12;
static const uint EASING_TYPE_EASEINOUTQUINTIC = 13;
static const uint EASING_TYPE_EASEINBACK = 14;
static const uint EASING_TYPE_EASEOUTBACK = 15;
static const uint EASING_TYPE_EASEINOUTBACK = 16;
static const uint EASING_TYPE_EASEINBOUNCE = 17;
static const uint EASING_TYPE_EASEOUTBOUNCE = 18;
static const uint EASING_TYPE_EASEINOUTBOUNCE = 19;

// イージングの速度タイプ
static const uint EASING_SPEEDTYPE_CONSTANT = 0;
static const uint EASING_SPEEDTYPE_SLOW0 = 1;
static const uint EASING_SPEEDTYPE_FAST0 = 2;
static const uint EASING_SPEEDTYPE_SLOW1 = 3;
static const uint EASING_SPEEDTYPE_FAST1 = 4;
static const uint EASING_SPEEDTYPE_SLOW2 = 5;
static const uint EASING_SPEEDTYPE_FAST2 = 6;

struct EasingValue
{
    RandVector3 startPoint;     // 始点
    RandVector3 endPoint;       // 終点
    uint easingType;            // イージングタイプ
    uint startSpeedType;        // 始点速度タイプ
    uint endSpeedType;          // 終点速度タイプ
    uint isMedianPoint;         // 中間点有効
    RandVector3 medianPoint;    // 中間点
};

struct PVA
{
    RandVector3 value;          // 値
    RandVector3 velocity;       // 速度
    RandVector3 acceleration;   // 加速度
};

struct EffectSRT
{
    uint type;          // タイプ
    float3 value;       // 値
    PVA pva;            // 値、速度、加速度
    EasingValue easing; // イージング値
};

struct EffectCommon
{
    uint emitCount;                 // 生成数
    uint isUnlimit;                 // 無限生成フラグ
    uint PosInfluenceType;          // 位置影響タイプ
    uint RotInfluenceType;          // 回転影響タイプ
    uint SclInfluenceType;          // スケール影響タイプ
    uint deleteLifetime;            // 寿命で削除
    uint deleteParentDeleted;       // 親削除で削除
    uint deleteAllChildrenDeleted;  // 全ての子削除で削除
    RandValue lifeTime;             // 生存時間
    RandValue emitTime;             // 生成時間
    RandValue emitStartTime;        // 生成開始時間
};

// FadeOutタイプ
static const uint FADEOUT_TYPE_NONE = 0;        // フェードアウトしない
static const uint FADEOUT_TYPE_ONLIFETIME = 1;  // 生存時間内でフェードアウト
static const uint FADEOUT_TYPE_DELETED = 2;     // 削除後にフェードアウト

// UVタイプ
static const uint UV_TYPE_STANDERD = 0;     // 標準UV
static const uint UV_TYPE_CONSTANT = 1;     // 固定UV
static const uint UV_TYPE_ANIMATION = 2;    // アニメーションUV
static const uint UV_TYPE_SCROLL = 3;       // スクロールUV

struct UVConstantValue
{
    float2 startPoint; // 開始UV
    float2 scale; // スケール
};

struct UVAnimationParameter
{
    float2 startPoint; // 開始UV
    float2 scale; // スケール
    uint oneTime; // 1回分の時間
    uint widthSheetCount; // 横の枚数
    uint heightSheetCount; // 縦の枚数
    uint isLoop; // ループフラグ
    RandValue startSheet; // 開始シート
    uint interpolation; // 補間フラグ
};

struct EffectDrawCommon
{
    uint materialType = 0; // マテリアルタイプ(現在は通常のみ)
    float emissive; // 発光値
    uint isFadeIn;                              // フェードイン(0:しない 1:する)
    uint isFadeOutType;                         // フェードアウトタイプ
    UVConstantValue constantValue;              // UV定数値
    UVAnimationParameter animationParameter;    // UVアニメーションパラメータ
};

struct EffectMeshTypeSprite
{
    float4 color;
    uint placement;
};
struct EffectTime
{
    float globalTime;
};

static const uint MESH_TYPE_NONE = 0;       // メッシュタイプなし
static const uint MESH_TYPE_SPRITE = 1;     // スプライト
static const uint MESH_TYPE_RIBBON = 2;     // リボン
static const uint MESH_TYPE_TRAIL = 3;      // トレイル
static const uint MESH_TYPE_RING = 4;       // リング
static const uint MESH_TYPE_MODEL = 5;      // モデル
static const uint MESH_TYPE_CUBE = 6;       // 立方体
static const uint MESH_TYPE_SPHERE = 7;     // 球


struct EffectDraw
{
    uint meshType;
};

struct EffectNode
{
    EffectCommon common;
    EffectSRT position;
    EffectSRT rotation;
    EffectSRT scale;
};

struct EffectNode
{
    EffectCommon common;
    EffectSRT position;
    EffectSRT rotation;
    EffectSRT scale;
    EffectDrawCommon drawCommon;
    EffectDraw draw;
    uint parentIndex;
};

struct TimeManager
{
    float globalTime;
    float maxTime;
    float deltaTime;
};


[numthreads(1, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
}