SamplerState ShadowSampler                   : K_SLOT_SHADOW_SAMPLER;
Texture2D ShadowAtlas                        : K_SLOT_SHADOW_ATLAS;
cbuffer ShadowPCF                            : K_SLOT_SHADOW_PCF
{
    int PcfFilterSize = 0;
    float PcfMinBias = 0.005;
    float PcfMaxBias = 0.05;
};

float DirectShadow(float3 lightDir, float3 positionLightSpace)
{
    int filterSize = PcfFilterSize;
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
                float depth = ShadowAtlas.Sample(ShadowSampler, positionLightSpace.xy + float2(x, y) * texelSize).r;
                shadow += (depth + bias) < posZ ? 0.0 : 1.0;
            }
        }
        shadow /= (filterSize * filterSize);
    }
    // mapping without PCF
    else {
        float depth = ShadowAtlas.Sample(ShadowSampler, positionLightSpace.xy).r;
        shadow = (depth + bias) < posZ ? 0.0 : 1.0;
    }

    return shadow;
}