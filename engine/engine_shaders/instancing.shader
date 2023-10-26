struct RenderInstance
{
    uint TransformIndex;
    uint MaterialIndex;
};

StructuredBuffer<RenderInstance> Instances : K_SLOT_INSTANCES;