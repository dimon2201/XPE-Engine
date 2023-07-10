struct SpotLight {
    float4 Position;
    float4 Direction;
    float4 Color;
    float Cutoff;
    float Outer;
    float Refraction;
};

StructuredBuffer<SpotLight> SpotLights : K_SLOT_SPOT_LIGHTS;
