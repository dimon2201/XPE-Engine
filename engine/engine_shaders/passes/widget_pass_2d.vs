#include ../types.shader
#include ../camera.shader

struct Widget
{
    float4x4 ModelMatrix;
    float4 Color;
    bool EnableTexture;
};
StructuredBuffer<Widget> Widgets  : K_SLOT_WIDGETS;

struct Char
{
    float GlyphSize;
    float Width;
    float Height;
    float Left;
    float Top;
    float AdvanceX;
    float AdvanceY;
    float AtlasXOffset;
    float AtlasYOffset;
};
StructuredBuffer<Char> Chars : K_SLOT_CHARS;

cbuffer TextBuffer : K_SLOT_TEXT
{
    float4x4 TextModelMatrix;
    float4 TextColor;
};

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
    float gamma : XPE_GAMMA;
    bool enableTexture : XPE_ENABLE_TEXTURE;
    bool enableFont : XPE_ENABLE_FONT;
    float2 glyphSize : XPE_GLYPH_SIZE;
    float2 glyphAtlasOffset : XPE_GLYPH_ATLAS_OFFSET;
};

VSOut vs_main(VSIn vsIn)
{
    VSOut vsOut;

    float4 positionNDC = float4(vsIn.positionLocal, 1.0f);

    if (TextColor.x != -1) {
        // draw widget as text

        Char c = Chars[vsIn.instanceIndex];

        float2 offsetScaled = float2(TextModelMatrix[0][0], TextModelMatrix[1][1]) * float2(c.Left, c.Top);
        float2 sizeScaled = float2(TextModelMatrix[0][0], TextModelMatrix[1][1]) * float2(c.Width, c.Height);

        float3 positionLocal = (vsIn.positionLocal * 0.5f) + 0.25f;

        positionNDC = float4(positionLocal, 1.0f);
        positionNDC *= float4(c.Width, c.Height, 1.0f, 1.0f);
        positionNDC = mul(TextModelMatrix, positionNDC);

        positionNDC.x += 0.5f * (offsetScaled.x + c.AdvanceX);
        positionNDC.y -= 0.5f * ((sizeScaled.y - offsetScaled.y) + c.AdvanceY);

        positionNDC.xy /= float2(ViewportWidth, ViewportHeight);
        positionNDC.xy = (positionNDC.xy * 2.0f) - 1.0f;

        vsOut.color = TextColor;
        vsOut.enableFont = true;
        vsOut.glyphSize = float2(c.Width, c.Height);
        vsOut.glyphAtlasOffset = float2(c.AtlasXOffset, c.AtlasYOffset);
        vsOut.enableTexture = false;
    }
    else {
        // draw widget

        Widget widget = Widgets[vsIn.instanceIndex];
        float4x4 modelMatrix = widget.ModelMatrix;

        positionNDC = mul(modelMatrix, positionNDC);
        positionNDC.xy /= float2(ViewportWidth, ViewportHeight);
        positionNDC.xy = (positionNDC.xy * 2.0f) - 1.0f;

        vsOut.color = widget.Color;
        vsOut.enableFont = false;
        vsOut.glyphSize = float2(0, 0);
        vsOut.glyphAtlasOffset = float2(0, 0);
        vsOut.enableTexture = widget.EnableTexture;
    }

    vsOut.uv = vsIn.uv;
    vsOut.positionNDC = positionNDC;
    vsOut.gamma = ViewportGamma;

    return vsOut;
}