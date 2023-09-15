float4 GammaCorrection(float4 color, float gamma) {
    return float4(pow(color.rgb, float3(gamma, gamma, gamma)), color.a);
}

float3 GammaCorrection(float3 color, float gamma) {
    return pow(color.rgb, float3(gamma, gamma, gamma));
}
