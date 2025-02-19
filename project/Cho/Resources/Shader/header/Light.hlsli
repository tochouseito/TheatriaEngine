
static const int MAX_DIRECTIONAL_LIGHTS = 10;

struct DirectionalLight
{
    float3 color;
    float intensity;
    float3 direction;
    uint active;
};

static const int MAX_POINT_LIGHTS = 10;

struct PointLight
{
    float3 color;
    float intensity;
    float3 position;
    float radius;
    float decay;
    uint active;
};

static const int MAX_SPOT_LIGHTS = 10;

struct SpotLight {
    float3 color;
    float intensity;
    float3 direction;
    float distance;
    float3 position;
    float decay;
    float cosAngle;
    float cosFalloffStart;
    uint active;
};

struct PunctualLights
{
    float3 ambientLight;
    DirectionalLight directionalLights[MAX_DIRECTIONAL_LIGHTS];
    PointLight pointLights[MAX_POINT_LIGHTS];
    SpotLight spotLights[MAX_SPOT_LIGHTS];
};