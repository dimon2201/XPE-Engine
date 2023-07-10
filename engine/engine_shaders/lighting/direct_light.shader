struct DirectLight {
    float4 Position;
    float4 Color;
};

StructuredBuffer<DirectLight> DirectLights : K_SLOT_DIRECT_LIGHTS;
