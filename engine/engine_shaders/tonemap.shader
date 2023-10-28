// Reinhard Tonemapper
float3 TonemapReinhard(in float3 color, in float gamma)
{
    color *= 16;
    color = color / (1 + color);
    return pow(color, 1 / gamma); // gamma
}

// Uncharted 2 Tonemapper
float3 TonemapUncharted2(in float3 x)
{
    float A = 0.15;
    float B = 0.50;
    float C = 0.10;
    float D = 0.20;
    float E = 0.02;
    float F = 0.30;

    return ((x*(A*x+C*B)+D*E)/(x*(A*x+B)+D*F))-E/F;
}

// UC2 Tonemapper
float3 TonemapUC2(in float3 color, in float gamma, in float exposure)
{
    float w = 11.2;

    color *= 16;  // Hardcoded Exposure Adjustment

    float3 current = TonemapUncharted2(exposure * color);

    float3 whiteScale = 1.0f / TonemapUncharted2(w);
    float3 ccolor = current * whiteScale;

    return pow(abs(ccolor), 1 / gamma); // gamma
}

// Filmic Tonemapper
float3 TonemapFilmic(in float3 color, in float gamma)
{
    color = max(0, color - 0.004f);
    color = (color * (6.2f * color + 0.5f)) / (color * (6.2f * color + 1.7f) + 0.06f);
    return pow(abs(color), 1 / gamma);
}

// Exp Tonemapper
float3 TonemapExp(in float3 color, in float gamma, in float exposure) {
    color = float3(1, 1, 1) - exp(-color * exposure);
    return color;
}