SamplerState ShadowSampler                   : K_SLOT_SHADOW_SAMPLER;
Texture2D ShadowAtlas                        : K_SLOT_SHADOW_ATLAS;

float DirectShadow(float3 lightDir, float4 positionLightSpace)
{
    float3 lightCoords = positionLightSpace.xyz / positionLightSpace.w;
    lightCoords = lightCoords * 0.5 + 0.5;

    if (lightCoords.z > 1.0)
        return 0;

    float currentDepth = lightCoords.z;

    float bias = max(0.025 * (1.0 - dot(N, lightDir)), 0.0005);

    float w;
    float h;
    ShadowAtlas.GetDimensions(w, h);
    float2 texelSize = 1.0 / float2(w, h);
    float shadow = 0.0;
    int sampleRadius = 2;

    for (int y = -sampleRadius; y <= sampleRadius; y++)
    {
        for (int x = -sampleRadius; x <= sampleRadius; x++)
        {
            float closestDepth = ShadowAtlas.Sample(ShadowSampler, lightCoords.xy + float2(x, y) * texelSize).r;
            if (currentDepth > closestDepth + bias) {
                shadow += 1.0;
            }
        }
    }
    shadow /= pow((sampleRadius * 2 + 1), 2);

    return shadow;
}