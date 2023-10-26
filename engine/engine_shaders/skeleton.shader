struct Bone
{
    float4x4 Transform;
};

StructuredBuffer<Bone> Bones : K_SLOT_BONES;