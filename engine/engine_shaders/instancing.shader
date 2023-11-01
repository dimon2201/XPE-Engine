struct RenderInstance
{
    uint TransformIndex;
    uint MaterialIndex;
    uint SkeletonIndex;
    float4x4 ModelMatrix;
    float4x4 NormalMatrix;
    float4x4 LightMatrix;
};

StructuredBuffer<RenderInstance> Instances : K_SLOT_INSTANCES;