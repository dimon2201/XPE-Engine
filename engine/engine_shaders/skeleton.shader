struct Bone
{
    float4x4 Transform;
};

StructuredBuffer<Bone> Skeletons : K_SLOT_SKELETONS;