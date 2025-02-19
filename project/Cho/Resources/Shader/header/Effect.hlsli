struct VertexData
{
    float4 position : POSITION;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
};

struct RandValue
{
    float first;
    float second;
};

struct LifeTime
{
    float time;
    uint isMedian;
    RandValue randValue;
};

struct EmitTime
{
    float time;
    uint isMedian;
    RandValue randValue;
};

struct EmitStartTime
{
    float time;
    uint isMedian;
    RandValue randValue;
};

struct EffectTime
{
    float globalTime; 
};

struct PVAVector3
{
    float3 first;
    float3 second;
    uint isMedian;
};

struct PVA
{
    PVAVector3 value;
    PVAVector3 velocity;
    PVAVector3 acceleration;
};

struct EffectPosition
{
    uint type;
    float3 value;
    PVA pva;
};

struct EffectRotation
{
    uint type;
    float3 value;
    PVA pva;
};

struct EffectScale
{
    uint type;
    float3 value;
    PVA pva;
};

struct EffectCommon
{
    uint maxCount;
    uint emitCount;
    uint PosInfluenceType;
    uint RotInfluenceType;
    uint SclInfluenceType;
    uint deleteLifetime;
    uint deleteParentDeleted;
    uint deleteAllChildrenDeleted;
    LifeTime lifeTime;
    EmitTime emitTime;
    EmitStartTime emitStartTime;
};

struct EffectDrawCommon
{
    float emissive;
    uint fadeOutType;
};

struct EffectMeshTypeSprite
{
    float4 color;
    uint placement;
};

struct EffectDraw
{
    uint meshType;
    EffectMeshTypeSprite meshSprite;
};

struct EffectNode
{
    EffectCommon common;
    EffectPosition position;
    EffectRotation rotation;
    EffectScale scale;
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