struct RenderInstance
{
    float4x4 ModelMatrix;
    float4x4 NormalMatrix;
    uint MaterialIndex;
    uint SkeletonIndex;
    uint LightIndex;
};

StructuredBuffer<RenderInstance> Instances : K_SLOT_INSTANCES;