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
Texture2DArray M_Emission : K_SLOT_EMISSION;

float4 GetAlbedo(uint materialIndex, float2 uv) {
    float mId = float(materialIndex);
    Material material = Materials[materialIndex];
    float4 albedo = material.BaseColor;
    if (material.EnableAlbedo) {
        albedo *= M_Albedo.Sample(S_Material, float3(uv, mId));
    }
    return albedo;
}

float3 GetNormal(uint materialIndex, float2 uv, float3 normal, float3x3 tbn) {
    float mId = float(materialIndex);
    Material material = Materials[materialIndex];
    if (material.EnableBumping) {
        float3 normalTangent = M_Bump.Sample(S_Material, float3(uv, mId)).xyz * 2.0 - 1.0;
        normal = normalize(mul(tbn, normalTangent));
    }
    return normal;
}

float GetMetallic(uint materialIndex, float2 uv) {
    float mId = float(materialIndex);
    Material material = Materials[materialIndex];
    float metallic = material.MetallicFactor;
    if (material.EnableMetallic) {
        metallic *= M_Metallic.Sample(S_Material, float3(uv, mId)).r;
    }
    return metallic;
}

float GetRoughness(uint materialIndex, float2 uv) {
    float mId = float(materialIndex);
    Material material = Materials[materialIndex];
    float roughness = material.RoughnessFactor;
    if (material.EnableRoughness) {
        roughness *= M_Roughness.Sample(S_Material, float3(uv, mId)).r;
    }
    return roughness;
}

float GetAO(uint materialIndex, float2 uv) {
    float mId = float(materialIndex);
    Material material = Materials[materialIndex];
    float ao = material.AOFactor;
    if (material.EnableAO) {
        ao *= M_AO.Sample(S_Material, float3(uv, mId)).r;
    }
    return ao;
}