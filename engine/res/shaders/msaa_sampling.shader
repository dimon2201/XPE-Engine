float SampleMSAATextureFloat(Texture2DMS<float> msaaTexture, float2 uv)
{
    uint width = 0;
    uint height = 0;
    uint sampleCount = 0;
    msaaTexture.GetDimensions(width, height, sampleCount);

    float averageColor = 0.0;
    for (int i = 0; i < int(sampleCount); i++)
    {
        averageColor += msaaTexture.Load(int2(uv * float2(width, height)), i);
    }
    averageColor *= 1.0 / float(sampleCount);

    return averageColor;
}

float3 SampleMSAATextureFloat3(Texture2DMS<float4> msaaTexture, float2 uv)
{
    uint width = 0;
    uint height = 0;
    uint sampleCount = 0;
    msaaTexture.GetDimensions(width, height, sampleCount);

    float3 averageColor = float3(0.0, 0.0, 0.0);
    for (int i = 0; i < int(sampleCount); i++)
    {
        averageColor += msaaTexture.Load(int2(uv * float2(width, height)), i).xyz;
    }
    averageColor *= 1.0 / float(sampleCount);

    return averageColor;
}

float4 SampleMSAATextureFloat4(Texture2DMS<float4> msaaTexture, float2 uv)
{
    uint width = 0;
    uint height = 0;
    uint sampleCount = 0;
    msaaTexture.GetDimensions(width, height, sampleCount);

    float4 averageColor = float4(0.0, 0.0, 0.0, 0.0);
    for (int i = 0; i < int(sampleCount); i++)
    {
        averageColor += msaaTexture.Load(int2(uv * float2(width, height)), i);
    }
    averageColor *= 1.0 / float(sampleCount);

    return averageColor;
}