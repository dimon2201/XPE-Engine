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
};

VSOut vs_main(VSIn vsIn)
{
    float2 position = vsIn.positionLocal.xy * 2.0;

    VSOut vsOut;
    vsOut.positionClip = float4(position, 0.0, 1.0);
    vsOut.uv = vsIn.uv;

    return vsOut;
}