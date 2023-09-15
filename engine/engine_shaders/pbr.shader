float3 Fresnel(float cosTheta, float3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

float3 FresnelRough(float cosTheta, float3 F0, float roughness)
{
    return F0 + (max(float3(1.0 - roughness, 1.0 - roughness, 1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

float DistributionGGX(float3 H, float roughness)
{
    float a   = roughness * roughness;
    float a2  = a * a;
    float NH  = max(dot(N, H), 0);
    float NH2 = NH * NH;
    float x = (NH2 * (a2 - 1.0) + 1.0);

    return a2 / (PI * x * x);
}

float GeometryShlickGGX(float NV, float roughness)
{
    float k = (roughness + 1.0) * (roughness + 1.0) / 8.0;
    return NV / (NV * (1.0 - k) + k);
}

float GeometrySmith(float roughness)
{
    float NV = max(dot(N, V), 0);
    float NL = max(dot(N, L), 0);
    return GeometryShlickGGX(NV, roughness) * GeometryShlickGGX(NL, roughness);
}

float3 Lambert(float3 kd, float3 albedo)
{
    return kd * albedo / PI;
}

float3 PBR(float3 lightColor, float radianceFactor, float3 albedo, float metallic, float roughness)
{
    float3 H = normalize(V + L);
    float3 radiance = lightColor * radianceFactor;

    // diffuse refraction/specular reflection ratio on surface
    float3 F0 = float3(0.04, 0.04, 0.04); // base reflection 0.04 covers most of dielectrics
    F0        = lerp(F0, albedo, metallic); // F0 = albedo color if it's a complete metal
    float3 F  = Fresnel(max(dot(H, V), 0), F0);

    // normal distribution halfway vector along rough surface
    float D = DistributionGGX(H, roughness);

    // geometry obstruction and geometry shadowing of microfacets
    float G = GeometrySmith(roughness);

    // BRDF Cook-Torrance approximation
    float y = 0.0001; // infinetely small number used when dot product N/V or N/L returns 0
    float3 specular = (D * G * F) / ( 4.0 * max(dot(N, V), 0) * max(dot(N, L), 0) + y);

    // light contribution to reflectance equation
    float3 ks = F;
    float3 kd = float3(1.0, 1.0, 1.0) - ks;
    kd *= 1.0 - metallic;

    // diffuse absorption/scatter using Lambert function
    float3 diffuse = Lambert(kd, albedo);

    // solve reflectance equation
    return (diffuse + specular) * radiance * max(dot(N, L), 0);
}