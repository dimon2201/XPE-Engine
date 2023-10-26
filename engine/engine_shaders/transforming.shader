struct Transform {
    float4x4 ModelMatrix;
    float4x4 NormalMatrix;
    float4x4 LightMatrix;
};

StructuredBuffer<Transform> Transforms : K_SLOT_TRANSFORMS;