#pragma once

struct RandValue
{
    float median;
    float amplitude;
};

struct RandVector3
{
    RandValue x;
    RandValue y;
    RandValue z;
};

struct PVA
{
    RandVector3 value;
    RandVector3 velocity;
    RandVector3 acceleration;
};

struct EasingValue
{
    RandVector3 startPoint;
    RandVector3 endPoint;
    uint easingType;
    uint startSpeedType;
    uint endSpeedType;
    uint isMedianPoint;
    RandVector3 medianPoint;
};

static const uint SRT_TYPE_STANDARD = 0;
static const uint SRT_TYPE_PVA = 1;
static const uint SRT_TYPE_EASING = 2;

struct EffectSRT
{
    uint type;
    float3 value;
    PVA pva;
    EasingValue easing;
};