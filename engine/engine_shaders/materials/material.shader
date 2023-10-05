struct Material {
    // base color
    float4 BaseColor;
    uint EnableAlbedo;
    // bumping
    uint EnableBumping;
    // parallax
    uint EnableParallax;
    float HeightScale;
    float ParallaxMinLayers;
    float ParallaxMaxLayers;
    // metallic
    float MetallicFactor;
    uint EnableMetallic;
    // roughness
    float RoughnessFactor;
    uint EnableRoughness;
    // ambient occlusion
    float AOFactor;
    uint EnableAO;
    // emission
    float3 EmissionColor;
    uint EnableEmission;
    // transparency
    uint EnableTransparency;
};

StructuredBuffer<Material> Materials : K_SLOT_MATERIALS;

SamplerState S_Material    : K_SLOT_MATERIAL_SAMPLER;

Texture2DArray M_Albedo    : K_SLOT_ALBEDO;
Texture2DArray M_Bump      : K_SLOT_BUMPING;
Texture2DArray M_Parallax  : K_SLOT_PARALLAX;
Texture2DArray M_Metallic  : K_SLOT_METALLIC;
Texture2DArray M_Roughness : K_SLOT_ROUGHNESS;
Texture2DArray M_AO        : K_SLOT_AO;
Texture2DArray M_Emission  : K_SLOT_EMISSION;

float4 GetAlbedo(uint materialIndex, float2 uv, in float gamma) {
    float mId = float(materialIndex);
    Material material = Materials[materialIndex];
    float4 albedo = material.BaseColor;
    if (material.EnableAlbedo) {
        albedo *= M_Albedo.Sample(S_Material, float3(uv, mId));
    }
    albedo = pow(albedo, gamma);
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

float2 GetParallax(uint materialIndex, float2 uv) {
    float mId = float(materialIndex);
    Material material = Materials[materialIndex];
    if (material.EnableParallax) {
        float heightScale = material.HeightScale;
        float minLayers = material.ParallaxMinLayers;
        float maxLayers = material.ParallaxMaxLayers;
        float layers = lerp(maxLayers, minLayers, abs(dot(float3(0.0, 0.0, 1.0), V)));

        // calculate the size of each layer
        float layerDepth = 1.0 / layers;

        // depth of current layer
        float currentLayerDepth = 0.0;

        // the amount to shift the texture coordinates per layer (from vector P)
        float2 P = V.xy / V.z * heightScale;
        float2 deltaUV = P / layers;
        float2 currentUV = uv;
        float currentParallaxValue = M_Parallax.Sample(S_Material, float3(currentUV, mId)).r;

        // todo(cheerwizard): too many iterations ~1024, need to be fixed!
//        while (currentLayerDepth < currentParallaxValue) {
//            // shift texture coordinates along direction of P
//            currentUV -= deltaUV;
//            // get depthmap value at current texture coordinates
//            currentParallaxValue = M_Parallax.Sample(S_Material, float3(currentUV, mId)).r;
//            // get depth of next layer
//            currentLayerDepth += layerDepth;
//        }

        // get texture coordinates before collision (reverse operations)
        float2 prevUV = currentUV + deltaUV;

        // get depth after and before collision for linear interpolation
        float afterDepth = currentParallaxValue - currentLayerDepth;
        float beforeDepth = M_Parallax.Sample(S_Material, float3(prevUV, mId)).r - currentLayerDepth + layerDepth;

        // interpolation of texture coordinates
        float weight = afterDepth / (afterDepth - beforeDepth);

        float2 result = prevUV * weight + currentUV * (1.0 - weight);

        if (result.x > 1.0 || result.y > 1.0 || result.x < 0.0 || result.y < 0.0)
            discard;

        return result;
    }
    return uv;
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

float3 GetEmission(uint materialIndex, float2 uv, in float gamma) {
    float mId = float(materialIndex);
    Material material = Materials[materialIndex];
    float3 emission = material.EmissionColor;
    if (material.EnableEmission) {
        emission *= M_Emission.Sample(S_Material, float3(uv, mId)).rgb;
    }
    emission = pow(emission, gamma);
    return emission;
}

bool GetTransparencyEnabled(uint materialIndex) {
    float mId = float(materialIndex);
    Material material = Materials[materialIndex];
    return material.EnableTransparency;
}