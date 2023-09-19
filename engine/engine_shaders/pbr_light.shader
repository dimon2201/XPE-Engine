float3 PBR(DirectLight directLight, float3 albedo, float metallic, float roughness)
{
    L = normalize(directLight.Position);
    float3 lightColor = directLight.Color;
    float directShadow = 0;
    float radianceFactor = 1.0 - directShadow;
    return PBR(lightColor, radianceFactor, albedo, metallic, roughness);
}

float3 PBR(PointLight pointLight, float3 albedo, float metallic, float roughness)
{
    L = normalize(pointLight.Position - W);
    float3 lightColor = pointLight.Color;
    float A = Attenuation(pointLight);
    float pointShadow = 0;
    float radianceFactor = A * (1.0 - pointShadow);

    return PBR(lightColor, radianceFactor, albedo, metallic, roughness);
}

float3 PBR(SpotLight spotLight, float3 albedo, float metallic, float roughness)
{
    L = normalize(spotLight.Position - W);
    float3 lightColor   = spotLight.Color;
    float3 S            = normalize(-spotLight.Direction);
    float theta         = dot(L, S);
    float cutoff        = spotLight.Cutoff;
    float gamma         = spotLight.Outer;
    float epsilon       = cutoff - gamma;
    float I             = clamp((theta - gamma) / epsilon, 0.0, 1.0);
    float radianceFactor = I * (1.0);

    return PBR(lightColor, radianceFactor, albedo, metallic, roughness);
}