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

StructuredBuffer<MaterialBufferData> MaterialBuffer : register(t0);

SamplerState S_Material : register(s0);

Texture2DArray M_Albedo : register(t1);
Texture2DArray M_Bump : register(t2);
Texture2DArray M_Parallax : register(t3);
Texture2DArray M_Metallic : register(t4);
Texture2DArray M_Roughness : register(t5);
Texture2DArray M_AO : register(t6);
Texture2DArray M_Emissions : register(t7);

float4 GetAlbedo(uint materialIndex, float2 texcoord) {
    float mId = float(materialIndex);
    MaterialBufferData Material = MaterialBuffer[materialIndex];
    float4 albedo = Material.BaseColor;
    if (Material.EnableAlbedo) {
        albedo *= M_Albedo.Sample(S_Material, float3(texcoord, mId));
    }
    return albedo;
}