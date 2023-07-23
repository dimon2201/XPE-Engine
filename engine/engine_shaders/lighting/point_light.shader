struct PointLight {
    float3 Position;
    float3 Color;
    float Constant;
    float Linear;
    float Quadratic;
};

StructuredBuffer<PointLight> PointLights : K_SLOT_POINT_LIGHTS;
