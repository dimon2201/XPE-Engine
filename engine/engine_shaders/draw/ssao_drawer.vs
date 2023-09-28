struct VSIn
{
    float3 positionLocal : XPE_POSITION;
    float2 uv : XPE_UV;
    float3 normal : XPE_NORMAL; // normalized
    float3 tangent : XPE_TANGENT; // normalized
};

struct VSOut
{
    float4 positionClip : SV_POSITION;
    float2 uv : XPE_UV;
    float ssaoDirectionCount : XPE_SSAO_DIRECTION_COUNT;
    float ssaoSampleCount : XPE_SSAO_SAMPLE_COUNT;
};

cbuffer SSAOBuffer : register(b1)
{
    float SSAODirectionCount;
    float SSAOSampleCount;
    float _pad[2];
};

VSOut vs_main(VSIn vsIn)
{
    float2 position = vsIn.positionLocal.xy * 2.0;

    VSOut vsOut;
    vsOut.positionClip = float4(position, 0.0, 1.0);
    vsOut.uv = float2(position.x * 0.5 + 0.5, 1.0 - (position.y * 0.5 + 0.5));
    vsOut.ssaoDirectionCount = SSAODirectionCount;
    vsOut.ssaoSampleCount = SSAOSampleCount;

    return vsOut;
}