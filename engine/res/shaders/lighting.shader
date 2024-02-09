struct DirectLight
{
    float3 Position;
    float3 Color;
    float4x4 ViewProjection;
};

StructuredBuffer<DirectLight> DirectLights : K_SLOT_DIRECT_LIGHTS;

struct PointLight
{
    float3 Position;
    float3 Color;
    float Constant;
    float Linear;
    float Quadratic;
    float4x4 ViewProjection;
};

StructuredBuffer<PointLight> PointLights : K_SLOT_POINT_LIGHTS;

float Attenuation(PointLight pointLight)
{
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
    float4x4 ViewProjection;
    float Near;
    float Far;
    float Constant;
    float Linear;
    float Quadratic;
};

StructuredBuffer<SpotLight> SpotLights : K_SLOT_SPOT_LIGHTS;

float Attenuation(float3 lightDir, SpotLight spotLight)
{
    float theta = dot(lightDir, normalize(-spotLight.Direction));
    float epsilon = (spotLight.InnerCutoff - spotLight.OuterCutoff);
    float intensity = clamp((theta - spotLight.OuterCutoff) / epsilon, 0.0, 1.0);
    float d = length(spotLight.Position - W);
    float q = spotLight.Quadratic;
    float l = spotLight.Linear;
    float c = spotLight.Constant;
    float attenuation = 1.0 / ( q * d * d + l * d + c );
    return attenuation * intensity;
}