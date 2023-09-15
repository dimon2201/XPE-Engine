struct PointLight
{
    float3 Position;
    float3 Color;
    float Constant;
    float Linear;
    float Quadratic;
};

StructuredBuffer<PointLight> PointLights : K_SLOT_POINT_LIGHTS;

float Attenuation(PointLight pointLight) {
    float d = length(pointLight.Position - W);
    float q = pointLight.Quadratic;
    float l = pointLight.Linear;
    float c = pointLight.Constant;
    return 1.0 / ( q * d * d + l * d + c );
}
