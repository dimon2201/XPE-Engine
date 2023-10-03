float SampleMSAATexture(Texture2DMS<float> texture, float2 texcoord)
{
    uint width = 0;
    uint height = 0;
    uint sampleCount = 0;
    texture.GetDimensions(width, height, sampleCount);

    float averageColor;
    for (int i = 0; i < sampleCount; i++)
    {
        averageColor += texture.Load(int3(texcoord * float2(width, height), i));
    }
    averageColor *= 1.0 / float(i);

    return averageColor;
}

float3 SampleMSAATexture(Texture2DMS<float4> texture, float2 texcoord)
{
    uint width = 0;
    uint height = 0;
    uint sampleCount = 0;
    texture.GetDimensions(width, height, sampleCount);

    float3 averageColor;
    for (int i = 0; i < sampleCount; i++)
    {
        averageColor += texture.Load(int3(texcoord * float2(width, height), i));
    }
    averageColor *= 1.0 / float(i);

    return averageColor;
}