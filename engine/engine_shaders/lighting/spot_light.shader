struct SpotLight
{
    float3 Position;
    float3 Direction;
    float3 Color;
    float Cutoff;
    float Outer;
    float4x4 DLSMatrix;
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
