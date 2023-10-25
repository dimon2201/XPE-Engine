struct Material {
    // albedo mapping
    float4 Albedo;
    bool EnableAlbedoMap;
    // normal mapping
    bool EnableNormalMap;
    // parallax occlusion mapping
    bool EnableParallaxMap;
    float ParallaxHeightScale;
    float ParallaxMinLayers;
    float ParallaxMaxLayers;
    // metal mapping
    float Metallness;
    bool EnableMetalMap;
    // roughness mapping
    float Roughness;
    bool EnableRoughnessMap;
    // ambient occlusion mapping
    float AO;
    bool EnableAOMap;
    // emission mapping
    float3 Emission;
    bool EnableEmissionMap;
};

StructuredBuffer<Material> Materials : K_SLOT_MATERIALS;

SamplerState MaterialSampler : K_SLOT_MATERIAL_SAMPLER;

Texture2DArray AlbedoMap    : K_SLOT_ALBEDO;
Texture2DArray NormalMap    : K_SLOT_BUMPING;
Texture2DArray ParallaxMap  : K_SLOT_PARALLAX;
Texture2DArray MetalMap     : K_SLOT_METALLIC;
Texture2DArray RoughnessMap : K_SLOT_ROUGHNESS;
Texture2DArray AOMap        : K_SLOT_AO;
Texture2DArray EmissionMap  : K_SLOT_EMISSION;

float4 GetAlbedo(uint materialIndex, float2 uv, in float gamma) {
    float mId = float(materialIndex);
    Material material = Materials[materialIndex];
    float4 albedo = material.Albedo;
    if (material.EnableAlbedoMap) {
        albedo *= AlbedoMap.Sample(MaterialSampler, float3(uv, mId));
        albedo = pow(albedo, gamma);
    }
    return albedo;
}

float3 GetNormal(uint materialIndex, float2 uv, float3 normal, float3x3 tbn) {
    float mId = float(materialIndex);
    Material material = Materials[materialIndex];
    if (material.EnableNormalMap) {
        float3 normalTangent = NormalMap.Sample(MaterialSampler, float3(uv, mId)).xyz * 2.0 - 1.0;
        normal = normalize(mul(tbn, normalTangent));
    }
    return normal;
}

float2 GetParallax(uint materialIndex, float2 uv) {
    float mId = float(materialIndex);
    Material material = Materials[materialIndex];
    if (material.EnableParallaxMap) {
        float heightScale = material.ParallaxHeightScale;
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
        float currentParallaxValue = ParallaxMap.Sample(MaterialSampler, float3(currentUV, mId)).r;

        // todo(cheerwizard): too many iterations ~1024, need to be fixed!
//        while (currentLayerDepth < currentParallaxValue) {
//            // shift texture coordinates along direction of P
//            currentUV -= deltaUV;
//            // get depthmap value at current texture coordinates
//            currentParallaxValue = ParallaxMap.Sample(MaterialSampler, float3(currentUV, mId)).r;
//            // get depth of next layer
//            currentLayerDepth += layerDepth;
//        }

        // get texture coordinates before collision (reverse operations)
        float2 prevUV = currentUV + deltaUV;

        // get depth after and before collision for linear interpolation
        float afterDepth = currentParallaxValue - currentLayerDepth;
        float beforeDepth = ParallaxMap.Sample(MaterialSampler, float3(prevUV, mId)).r - currentLayerDepth + layerDepth;

        // interpolation of texture coordinates
        float weight = afterDepth / (afterDepth - beforeDepth);

        float2 result = prevUV * weight + currentUV * (1.0 - weight);

        if (result.x > 1.0 || result.y > 1.0 || result.x < 0.0 || result.y < 0.0)
            discard;

        return result;
    }
    return uv;
}

float GetMetallness(uint materialIndex, float2 uv) {
    float mId = float(materialIndex);
    Material material = Materials[materialIndex];
    float metallic = material.Metallness;
    if (material.EnableMetalMap) {
        metallic *= MetalMap.Sample(MaterialSampler, float3(uv, mId)).r;
    }
    return metallic;
}

float GetRoughness(uint materialIndex, float2 uv) {
    float mId = float(materialIndex);
    Material material = Materials[materialIndex];
    float roughness = material.Roughness;
    if (material.EnableRoughnessMap) {
        roughness *= RoughnessMap.Sample(MaterialSampler, float3(uv, mId)).r;
    }
    return roughness;
}

float GetAO(uint materialIndex, float2 uv) {
    float mId = float(materialIndex);
    Material material = Materials[materialIndex];
    float ao = material.AO;
    if (material.EnableAOMap) {
        ao *= AOMap.Sample(MaterialSampler, float3(uv, mId)).r;
    }
    return ao;
}

float3 GetEmission(uint materialIndex, float2 uv, in float gamma) {
    float mId = float(materialIndex);
    Material material = Materials[materialIndex];
    float3 emission = material.Emission;
    if (material.EnableEmissionMap) {
        emission *= EmissionMap.Sample(MaterialSampler, float3(uv, mId)).rgb;
        emission = pow(emission, gamma);
    }
    return emission;
}