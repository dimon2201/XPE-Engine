struct PointLight {
    float4 Position;
    float4 Color;
    float Constant;
    float Linear;
    float Quadratic;
    float Refraction;
};

StructuredBuffer<PointLight> PointLights : K_SLOT_POINT_LIGHTS;
