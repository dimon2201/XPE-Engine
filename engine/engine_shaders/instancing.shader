struct RenderInstance
{
    float4x4 ModelMatrix;
    float4x4 NormalMatrix;
    uint MaterialIndex;
    uint SkeletonIndex;
    uint LightIndex;
    uint ShadowCasterCount;
    float4 ShadowCasters[5];
};

StructuredBuffer<RenderInstance> Instances : K_SLOT_INSTANCES;