#include ../types.shader

struct Widget
{
    float4x4 ModelMatrix;
    float4x4 Projection;
    float4 Color;
    bool HasTexture;
};
StructuredBuffer<Widget> Widgets : K_SLOT_WIDGETS;

struct VSIn
{
    float3 positionLocal : XPE_POSITION;
    float2 uv : XPE_UV;
    uint instanceIndex : SV_InstanceID;
};

struct VSOut
{
    float4 positionOut : SV_POSITION;
    float2 uv : XPE_UV2;
    float4 color : XPE_WIDGET_COLOR;
    bool hasTexture : XPE_ENABLE_TEXTURE;
};

VSOut vs_main(VSIn vsIn)
{
    VSOut vsOut;

    float4 positionOut = float4(vsIn.positionLocal, 1.0f);

    Widget widget = Widgets[vsIn.instanceIndex];
    float4x4 modelMatrix = widget.ModelMatrix;
    float4x4 projection = widget.Projection;

    positionOut = mul(modelMatrix, positionOut);
    positionOut = mul(projection, positionOut);

    vsOut.positionOut = positionOut;
    vsOut.uv = vsIn.uv;
    vsOut.color = widget.Color;
    vsOut.hasTexture = widget.HasTexture;

    return vsOut;
}