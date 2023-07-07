struct MaterialBufferData {
    // base color
    float4 BaseColor;
    bool EnableAlbedo;
    // bumping
    bool EnableBumping;
    // parallax
    bool EnableParallax;
    float HeightScale;
    float ParallaxMinLayers;
    float ParallaxMaxLayers;
    // metallic
    float MetallicFactor;
    bool EnableMetallic;
    // roughness
    float RoughnessFactor;
    bool EnableRoughness;
    // ambient occlusion
    float AOFactor;
    bool EnableAO;
    // emission
    float3 EmissionColor;
    bool EnableEmission;
};

cbuffer MaterialBuffer : register(b1)
{
    MaterialBufferData Material;
};

SamplerState S_Material : register(s0);

Texture2D M_Albedo : register(t0);
Texture2D M_Bump : register(t1);
Texture2D M_Parallax : register(t2);
Texture2D M_Metallic : register(t3);
Texture2D M_Roughness : register(t4);
Texture2D M_AO : register(t5);
Texture2D M_Emission : register(t6);
