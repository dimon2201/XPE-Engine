float4 GammaCorrection(float4 color, float gamma) {
    return float4(pow(color.rgb, float3(gamma, gamma, gamma)), color.a);
}

float3 GammaCorrection(float3 color, float gamma) {
    return pow(color.rgb, float3(1 / gamma, 1 / gamma, 1 / gamma));
}

float3 ToneMapping(float3 color, float exposure) {
    return float3(1, 1, 1) - exp(-color * exposure);
}
