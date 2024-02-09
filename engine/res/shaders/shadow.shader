SamplerState ShadowSampler                   : K_SLOT_SHADOW_SAMPLER;
Texture2D ShadowAtlas                        : K_SLOT_SHADOW_ATLAS;
cbuffer ShadowPCF                            : K_SLOT_SHADOW_PCF
{
    float PcfMinBias;
    float PcfMaxBias;
    bool EnablePCF;
};
Texture2D ShadowCircleFilter                 : K_SLOT_SHADOW_CIRCLE_FILTER;

float ShadowMapping(
    float3 lightDir,
    float3 positionLightSpace,
    Texture2D shadowMap,
    SamplerState shadowSampler
) {
    float posZ = positionLightSpace.z;
    float bias = max(PcfMaxBias * (1.0 - dot(N, lightDir)), PcfMinBias);
    float shadow = 0.0;

    // mapping with PCF
    if (EnablePCF) {
        int3 offsetCoord;
        float size;
        int filterSize = 3;
        int samplesDiv2 = filterSize * filterSize / 2;
        float w;
        float h;
        ShadowAtlas.GetDimensions(w, h);
        float2 texelSize = 1.0 / float2(w, h);
        [loop]
        for (int x = -1; x <= 1; x++)
        {
            [loop]
            for (int y = -1; y <= 1; y++)
            {
                float depth = shadowMap.Sample(shadowSampler, positionLightSpace.xy + float2(x, y) * texelSize).r;
                shadow += (depth + bias) < posZ ? 0.0 : 1.0;
            }
        }
        shadow /= 9;
    }
    // mapping without PCF
    else {
        float depth = shadowMap.Sample(shadowSampler, positionLightSpace.xy).r;
        shadow = (depth + bias) < posZ ? 0.0 : 1.0;
    }

    return shadow;
}

float GetDirectionalShadow(float3 lightDir, float3 positionLightSpace)
{
    return ShadowMapping(lightDir, positionLightSpace, ShadowAtlas, ShadowSampler);
}

float GetPointShadow(float3 lightDir, float3 lightDistance)
{
    return ShadowMapping(lightDir, lightDistance, ShadowAtlas, ShadowSampler);
}

float GetSpotShadow(float3 lightDir, float3 positionLightSpace)
{
    return ShadowMapping(lightDir, positionLightSpace, ShadowAtlas, ShadowSampler);
}