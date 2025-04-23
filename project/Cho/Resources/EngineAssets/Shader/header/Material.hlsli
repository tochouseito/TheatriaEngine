struct Material {
    float4 color;
    int enableLighting;
    int enableTexture;
    int textureID;
    float4x4 matUV;
    float shininess;
};