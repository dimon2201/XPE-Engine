struct Transform {
    float4x4 Matrix;
};

StructuredBuffer<Transform> Transforms : K_SLOT_TRANSFORMS;

struct Transform2D {
    float3x3 Matrix;
};

StructuredBuffer<Transform2D> Transforms2D : K_SLOT_TRANSFORMS2D;
