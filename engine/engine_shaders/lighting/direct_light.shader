struct DirectLight
{
    float3 Position;
    float3 Color;
    float4x4 DLSMatrix;
};

StructuredBuffer<DirectLight> DirectLights : K_SLOT_DIRECT_LIGHTS;
