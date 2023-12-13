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

    return a2 / (K_PI * x * x);
}

float GeometryShlickGGX(float dotN, float roughness)
{
    float k = (roughness + 1.0) * (roughness + 1.0) / 8.0;
    return dotN / (dotN * (1.0 - k) + k);
}

float GeometrySmith(float roughness, float dotNV, float dotNL)
{
    return GeometryShlickGGX(dotNV, roughness) * GeometryShlickGGX(dotNL, roughness);
}

float3 Lambert(float3 kd, float3 albedo)
{
    return kd * albedo / K_PI;
}

float3 PBR(float3 lightColor, float radianceFactor, float3 albedo, float metallness, float roughness)
{
    float3 H = normalize(V + L);
    float3 radiance = lightColor * radianceFactor;
    float dotNV = max(dot(N, V), 0);
    float dotNL = max(dot(N, L), 0);

    // diffuse refraction/specular reflection ratio on surface
    float3 F0 = float3(0.04, 0.04, 0.04); // base reflection 0.04 covers most of dielectrics
    F0        = lerp(F0, albedo, metallness); // F0 = albedo color if it's a complete metal
    float3 F  = Fresnel(max(dot(H, V), 0), F0);

    // normal distribution halfway vector along rough surface
    float D = DistributionGGX(H, roughness);

    // geometry obstruction and geometry shadowing of microfacets
    float G = GeometrySmith(roughness, dotNV, dotNL);

    // BRDF Cook-Torrance approximation
    float y = 0.0001; // infinetely small number used when dot product N/V or N/L returns 0
    float3 specular = (D * G * F) / (4.0 * dotNV * dotNL + y);

    // light contribution to reflectance equation
    float3 ks = F;
    float3 kd = float3(1.0, 1.0, 1.0) - ks;
    kd *= 1.0 - metallness;

    // diffuse absorption/scatter using Lambert function
    float3 diffuse = Lambert(kd, albedo);

    // solve reflectance equation
    return (diffuse + specular) * radiance * dotNL;
}

float3 PBR(DirectLight directLight, float3 albedo, float metallness, float roughness)
{
    L = normalize(directLight.Position);
    float3 lightColor = directLight.Color;
    float directShadow = GetDirectionalShadow(L, ToLightSpace(W, directLight.ViewProjection));
    float radianceFactor = directShadow;
    return PBR(lightColor, radianceFactor, albedo, metallness, roughness);
}

float3 PBR(PointLight pointLight, float3 albedo, float metallness, float roughness)
{
    L = normalize(pointLight.Position - W);
    float3 lightColor = pointLight.Color;
    float A = Attenuation(pointLight);
    float pointShadow = 1.0;
    float radianceFactor = A * pointShadow;

    return PBR(lightColor, radianceFactor, albedo, metallness, roughness);
}

float3 PBR(SpotLight spotLight, float3 albedo, float metallness, float roughness)
{
    L = normalize(spotLight.Position - W);
    float3 lightColor = spotLight.Color;
    float A = Attenuation(spotLight);
    float spotShadow = GetSpotShadow(L, ToLightSpace(W, spotLight.ViewProjection));
    float radianceFactor = A * spotShadow;

    return PBR(lightColor, radianceFactor, albedo, metallness, roughness);
}