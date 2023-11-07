float3 GammaCorrection(in float3 color, in float gamma) {
    return pow(abs(color), 1 / gamma);
}

float4 GammaCorrection(in float4 color, in float gamma) {
    return float4(pow(abs(color.rgb), 1 / gamma), color.a);
}