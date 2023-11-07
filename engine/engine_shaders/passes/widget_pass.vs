#include ../types.shader
#include ../camera.shader

struct Widget
{
    float4x4 ModelMatrix;
    float4 Color;
    bool EnableTexture;
};
StructuredBuffer<Widget> Widgets  : K_SLOT_WIDGETS;

struct VSIn
{
    float3 positionLocal : XPE_POSITION;
    float2 uv : XPE_UV;
    uint instanceIndex : SV_InstanceID;
};

struct VSOut
{
    float2 uv : XPE_UV2;
    float4 positionNDC : SV_POSITION;
    float4 color : XPE_WIDGET_COLOR;
    bool enableTexture : XPE_WIDGET_ENABLE_TEXTURE;
    float gamma : XPE_GAMMA;
};

VSOut vs_main(VSIn vsIn)
{
    VSOut vsOut;

    Widget widget = Widgets[vsIn.instanceIndex];
    float4x4 modelMatrix = widget.ModelMatrix;

    float4 positionNDC = float4(vsIn.positionLocal, 1.0f);
    positionNDC = mul(modelMatrix, positionNDC);
    positionNDC.xy /= float2(ViewportWidth, ViewportHeight);
    positionNDC.xy = (positionNDC.xy * 2.0f) - 1.0f;

    vsOut.uv = vsIn.uv;
    vsOut.positionNDC = positionNDC;
    vsOut.color = widget.Color;
    vsOut.enableTexture = widget.EnableTexture;
    vsOut.gamma = ViewportGamma;

    return vsOut;
}