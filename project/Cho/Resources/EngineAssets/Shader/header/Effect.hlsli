struct RandValue {
    float median;       // 中央値
    float amplitude;    // 振幅
};

struct RandVector3 {
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

struct EasingValue {
    RandVector3 startPoint;     // 始点
    RandVector3 endPoint;       // 終点
    uint easingType;            // イージングタイプ
    uint startSpeedType;        // 始点速度タイプ
    uint endSpeedType;          // 終点速度タイプ
    uint isMedianPoint;         // 中間点有効
    RandVector3 medianPoint;    // 中間点
};

struct PVA {
    RandVector3 value;          // 値
    RandVector3 velocity;       // 速度
    RandVector3 acceleration;   // 加速度
};

struct EffectSRT {
    uint type;          // タイプ
    float3 value;       // 値
    PVA pva;            // 値、速度、加速度
    EasingValue easing; // イージング値
};

struct EffectCommon {
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

struct UVConstantValue {
    float2 startPoint;  // 開始UV
    float2 scale;       // スケール
};

struct UVAnimationParameter {
    float2 startPoint;      // 開始UV
    float2 scale;           // スケール
    uint oneTime;           // 1回分の時間
    uint widthSheetCount;   // 横の枚数
    uint heightSheetCount;  // 縦の枚数
    uint isLoop;            // ループフラグ
    RandValue startSheet;   // 開始シート
    uint interpolation;     // 補間フラグ
};

struct EffectDrawCommon {
    uint materialType = 0;                      // マテリアルタイプ(現在は通常のみ)
    float emissive;                             // 発光値
    uint isFadeIn;                              // フェードイン(0:しない 1:する)
    uint isFadeOutType;                         // フェードアウトタイプ
    UVConstantValue constantValue;              // UV定数値
    UVAnimationParameter animationParameter;    // UVアニメーションパラメータ
};

static const uint MESH_TYPE_NONE = 0;       // メッシュタイプなし
static const uint MESH_TYPE_SPRITE = 1;     // スプライト
static const uint MESH_TYPE_RIBBON = 2;     // リボン
static const uint MESH_TYPE_TRAIL = 3;      // トレイル
static const uint MESH_TYPE_RING = 4;       // リング
static const uint MESH_TYPE_MODEL = 5;      // モデル
static const uint MESH_TYPE_CUBE = 6;       // 立方体
static const uint MESH_TYPE_SPHERE = 7;     // 球

static const uint COLOR_TYPE_CONSTANT = 0;  // 固定
static const uint COLOR_TYPE_RANDOM = 1;    // ランダム
static const uint COLOR_TYPE_EASING = 2;    // イージング

struct RandColor {
    RandValue r; // 赤
    RandValue g; // 緑
    RandValue b; // 青
    RandValue a; // アルファ
};

struct EasingColor {
    float4 startMinColor;   // 始点最小色
    float4 startMaxColor;   // 始点最大色
    float4 endMinColor;     // 終点最小色
    float4 endMaxColor;     // 終点最大色
    uint startSpeedType;    // 始点速度タイプ
    uint endSpeedType;      // 終点速度タイプ
};

static const uint PLACEMENT_TYPE_BILLBOARD = 0;     // ビルボード
static const uint PLACEMENT_TYPE_BILLBOARDY = 1;    // Y軸ビルボード
static const uint PLACEMENT_TYPE_BILLBOARDXY = 2;   // Z軸回転ビルボード
static const uint PLACEMENT_TYPE_CONSTANT = 3;      // 固定

static const uint VERTEX_COLOR_TYPE_STANDARD = 0; // 標準
static const uint VERTEX_COLOR_TYPE_CONSTANT = 1; // 固定

static const uint VERTEX_TYPE_STANDARD = 0; // 標準
static const uint VERTEX_TYPE_CONSTANT = 1; // 固定

struct SpriteVertexColor {
    float4 leftBottom;  // 左下色
    float4 rightBottom; // 右下色
    float4 leftTop;     // 左上色
    float4 rightTop;    // 右上色
};

struct SpriteVertexPosition {
    float2 leftBottom;  // 左下座標
    float2 rightBottom; // 右下座標
    float2 leftTop;     // 左上座標
    float2 rightTop;    // 右上座標
};

struct EffectSprite {
    uint ColorType;                         // 色タイプ
    float4 color;                           // 色
    RandColor randColor;                    // ランダム色
    EasingColor easingColor;                // イージング色
    uint placement;                         // 配置方法
    uint VertexColorType;                   // 頂点色タイプ
    SpriteVertexColor vertexColor;          // 頂点色
    SpriteVertexPosition vertexPosition;    // 頂点座標
};

// ノード
struct EffectNode {
    EffectCommon common;            // 共通情報
    EffectSRT position;             // 位置
    EffectSRT rotation;             // 回転
    EffectSRT scale;                // スケール
    EffectDrawCommon drawCommon;    // 描画共通情報
    uint meshType;                  // メッシュタイプ
};

struct TimeManager {
    float globalTime;
    float maxTime;
    float deltaTime;
};