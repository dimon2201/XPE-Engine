float4 GammaCorrect(float4 color, float gamma) {
    return float4(pow(color.rgb, float3(gamma, gamma, gamma)), color.a);
}