struct Material {
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

StructuredBuffer<Material> Materials : K_SLOT_MATERIALS;

SamplerState S_Material : register(s0);

Texture2DArray M_Albedo    : K_SLOT_ALBEDO;
Texture2DArray M_Bump      : K_SLOT_BUMPING;
Texture2DArray M_Parallax  : K_SLOT_PARALLAX;
Texture2DArray M_Metallic  : K_SLOT_METALLIC;
Texture2DArray M_Roughness : K_SLOT_ROUGHNESS;
Texture2DArray M_AO        : K_SLOT_AO;
Texture2DArray M_Emissions : K_SLOT_EMISSION;

float4 GetAlbedo(uint materialIndex, float2 texcoord) {
    float mId = float(materialIndex);
    Material material = Materials[materialIndex];
    float4 albedo = material.BaseColor;
    if (material.EnableAlbedo) {
        albedo *= M_Albedo.Sample(S_Material, float3(texcoord, mId));
    }
    return albedo;
}