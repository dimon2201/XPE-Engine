struct VSOut
{
    float4 positionClip : SV_POSITION;
    float2 uv : XPE_UV;
    float ssaoDirectionCount : XPE_SSAO_DIRECTION_COUNT;
    float ssaoSampleCount : XPE_SSAO_SAMPLE_COUNT;
    float ssaoIntensity : XPE_SSAO_INTENSITY;
};

cbuffer SSAOBuffer : register(b0)
{
    float SSAODirectionCount;
    float SSAOSampleCount;
    float SSAOIntensity;
    float _pad[1];
};

VSOut vs_main(uint vertexIndex : SV_VertexID)
{
    VSOut vsOut;

    if (vertexIndex == 0) { vsOut.positionClip = float4(-1.0, -1.0, 0.0, 1.0); vsOut.uv = float2(0.0, 1.0); }
    if (vertexIndex == 1) { vsOut.positionClip = float4(-1.0, 1.0, 0.0, 1.0); vsOut.uv = float2(0.0, 0.0); }
    if (vertexIndex == 2) { vsOut.positionClip = float4(1.0, -1.0, 0.0, 1.0); vsOut.uv = float2(1.0, 1.0); }
    if (vertexIndex == 3) { vsOut.positionClip = float4(1.0, 1.0, 0.0, 1.0); vsOut.uv = float2(1.0, 0.0); }

    vsOut.ssaoDirectionCount = SSAODirectionCount;
    vsOut.ssaoSampleCount = SSAOSampleCount;
    vsOut.ssaoIntensity = SSAOIntensity;

    return vsOut;
}