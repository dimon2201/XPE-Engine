struct DirectLight
{
    float3 Position;
    float3 Color;
};

StructuredBuffer<DirectLight> DirectLights : K_SLOT_DIRECT_LIGHTS;

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

struct SpotLight
{
    float3 Position;
    float3 Direction;
    float3 Color;
    float Cutoff;
    float Outer;
};

StructuredBuffer<SpotLight> SpotLights : K_SLOT_SPOT_LIGHTS;

float Attenuation(SpotLight spotLight) {
    float3 lightDir     = normalize(spotLight.Position - W);
    float3 spotDir      = normalize(-spotLight.Direction);
    float theta         = dot(lightDir, spotDir);
    float cutoff        = spotLight.Cutoff;
    float gamma         = spotLight.Outer;
    float epsilon       = cutoff - gamma;
    return clamp((theta - gamma) / epsilon, 0.0, 1.0);
}
