struct RenderInstance
{
    float4x4 ModelMatrix;
    float4x4 NormalMatrix;
    uint MaterialIndex;
    uint SkeletonIndex;
};

StructuredBuffer<RenderInstance> Instances : K_SLOT_INSTANCES;