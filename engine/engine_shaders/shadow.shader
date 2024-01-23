SamplerState ShadowSampler                   : K_SLOT_SHADOW_SAMPLER;
Texture2D ShadowAtlas                        : K_SLOT_TEXTURE_SHADOW_ATLAS;
cbuffer ShadowPCF                            : K_SLOT_SHADOW_PCF
{
    int PcfFilterSize = 0;
    float PcfMinBias = 0.005;
    float PcfMaxBias = 0.05;
};

float ShadowMapping(
    float3 lightDir,
    float3 positionLightSpace,
    Texture2D shadowMap,
    SamplerState shadowSampler,
    int filterSize
) {
    float posZ = positionLightSpace.z;
    float bias = max(PcfMaxBias * (1.0 - dot(N, lightDir)), PcfMinBias);
    float shadow = 0.0;

    // mapping with PCF
    if (filterSize > 0) {
        float w;
        float h;
        ShadowAtlas.GetDimensions(w, h);
        float2 texelSize = 1.0 / float2(w, h);
        [loop]
        for (int x = -filterSize; x <= filterSize; x++)
        {
            [loop]
            for (int y = -filterSize; y <= filterSize; y++)
            {
                float depth = shadowMap.Sample(shadowSampler, positionLightSpace.xy + float2(x, y) * texelSize).r;
                shadow += (depth + bias) < posZ ? 0.0 : 1.0;
            }
        }
        shadow /= (filterSize * filterSize);
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
    return ShadowMapping(lightDir, positionLightSpace, ShadowAtlas, ShadowSampler, PcfFilterSize);
}

float GetSpotShadow(float3 lightDir, float3 positionLightSpace)
{
    return ShadowMapping(lightDir, positionLightSpace, ShadowAtlas, ShadowSampler, PcfFilterSize);
}



uint2 GetShadowAtlasSize() 
{
    uint width = 0;
    uint height = 0;
    ShadowAtlas.GetDimensions(width, height);

    return uint2(width, height);
}

bool IsShadowed(int lightIndex, float4 shadowCaster, float3 worldPosition)
{
    uint2 shadowAtlasSize = GetShadowAtlasSize();
    float2 bottomLeft = float2(shadowCaster.x, shadowCaster.y) / float2(shadowAtlasSize);
    float2 topRight = float2(shadowCaster.z, shadowCaster.w) / float2(shadowAtlasSize);
    float2 shadowMapSize = topRight - bottomLeft;

    float4 viewPosition = mul(DirectLights[lightIndex].View, float4(worldPosition, 1.0));
    float4 clipPosition = mul(DirectLights[lightIndex].Projection, float4(viewPosition.xyz, 1.0));

    if (clipPosition.x < -1.0 || clipPosition.y > 1.0 ||
        clipPosition.y < -1.0 || clipPosition.y > 1.0) {
        return false;
    }

    clipPosition.x = (clipPosition.x / 2.0) + 0.5;
    clipPosition.y = (clipPosition.y / -2.0) + 0.5;

    clipPosition.x = (clipPosition.x * shadowMapSize.x) + bottomLeft.x;
    clipPosition.y = (clipPosition.y * shadowMapSize.y) + bottomLeft.y;
    
    float pointDepth = viewPosition.z / 1024.0;
    float shadowMapDepth = ShadowAtlas.Load(int3(float2(shadowAtlasSize.x, shadowAtlasSize.y) * clipPosition.xy, 0)).x;
    if (shadowMapDepth < pointDepth && shadowMapDepth > 0.0) {
        return true;
    } else {
        return false;
    }
}