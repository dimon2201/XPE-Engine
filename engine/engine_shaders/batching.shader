struct RenderInstance
{
    uint TransformIndex;
    uint CameraIndex;
    uint MaterialIndex;
};

StructuredBuffer<RenderInstance> Instances : K_SLOT_INSTANCES;

struct RenderInstance2D
{
    uint TransformIndex;
    uint CameraIndex;
};

StructuredBuffer<RenderInstance2D> Instances2D : K_SLOT_INSTANCES2D;
