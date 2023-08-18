struct SpotLight
{
    float3 Position;
    float3 Direction;
    float3 Color;
    float Cutoff;
    float Outer;
};

StructuredBuffer<SpotLight> SpotLights : K_SLOT_SPOT_LIGHTS;
