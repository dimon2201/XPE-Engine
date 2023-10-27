#include ../types.shader
#include ../camera.shader

struct VSIn
{
    float3 positionLocal : XPE_POSITION;
    float2 uv : XPE_UV;
    uint instanceIndex : SV_InstanceID;
};

struct VSOut
{
    float4 positionClip : SV_POSITION;
    float3 positionLocal : XPE_POSITION_LOCAL;
};

VSOut vs_main(VSIn vsIn)
{
    VSOut vsOut;

    Camera camera = Cameras[0];

    float3x3 v = float3x3(camera.View[0].xyz, camera.View[1].xyz, camera.View[2].xyz);
    float4x4 view;
    view[0] = float4(v[0], 0);
    view[1] = float4(v[1], 0);
    view[2] = float4(v[2], 0);
    view[3] = float4(0, 0, 0, 1);
    float4 positionView  = mul(camera.View, float4(vsIn.positionLocal, 1.0));
    float4 positionClip  = mul(camera.Projection, positionView);

    vsOut.positionClip = positionClip;
    vsOut.positionLocal = vsIn.positionLocal;

    return vsOut;
}
