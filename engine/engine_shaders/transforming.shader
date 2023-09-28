struct Transform {
    float4x4 ModelMatrix;
    float4x4 NormalMatrix;
    float4x4 DLSMatrix;
};

StructuredBuffer<Transform> Transforms : K_SLOT_TRANSFORMS;

struct Transform2D {
    float3x3 ModelMatrix;
};

StructuredBuffer<Transform2D> Transforms2D : K_SLOT_TRANSFORMS2D;
