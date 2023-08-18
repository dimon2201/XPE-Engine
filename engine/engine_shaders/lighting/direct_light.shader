struct DirectLight
{
    float3 Position;
    float3 Color;
};

StructuredBuffer<DirectLight> DirectLights : K_SLOT_DIRECT_LIGHTS;
