struct RenderInstance
{
    uint TransformIndex;
    uint CameraIndex;
    uint MaterialIndex;
};

StructuredBuffer<RenderInstance> Instances : K_SLOT_INSTANCES;