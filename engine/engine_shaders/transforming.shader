struct Transform {
    float4x4 ModelMatrix;
    float4x4 NormalMatrix;
};

StructuredBuffer<Transform> Transforms : K_SLOT_TRANSFORMS;