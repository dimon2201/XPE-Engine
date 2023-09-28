struct ShadowFilter {
    int FilterSize;
};

StructuredBuffer<ShadowFilter> ShadowFilters : K_SLOT_SHADOW_FILTER;
SamplerState ShadowSampler                   : K_SLOT_SHADOW_SAMPLER;
Texture2D ShadowMap                          : K_SLOT_SHADOW_MAP;
Texture2D ShadowCoords                       : K_SLOT_SHADOW_COORDS;

float DirectShadow(float3 lightPos)
{
    float3 projCoords = ShadowCoords.Sample(ShadowSampler, UV).xyz;

    if (projCoords.z > 1.0)
        return 0;

    float currentDepth = projCoords.z;

    float bias = max(0.05 * (1.0 - dot(N, lightPos)), 0.005);

    float w;
    float h;
    ShadowMap.GetDimensions(w, h);
    float2 texelSize = 1.0 / float2(w, h);
    float shadow = 0.0;
    int shadowFilterSize = 9;
    int halfFilterSize = shadowFilterSize / 2;

    for (int y = -halfFilterSize; y <= -halfFilterSize + shadowFilterSize; y++)
    {
        for (int x = -halfFilterSize; x <= -halfFilterSize + shadowFilterSize; x++)
        {
            float pcf = ShadowMap.Sample(ShadowSampler, projCoords.xy + float2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcf ? 1.0 : 0.0;
        }
    }
    shadow /= float(pow(shadowFilterSize, 2));

    return shadow;
}