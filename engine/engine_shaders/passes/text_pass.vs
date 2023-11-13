#include ../types.shader

struct Char
{
    uint TextIndex;
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

struct Text
{
    float4x4 ModelMatrix;
    float4x4 Projection;
    float4 Color;
};
StructuredBuffer<Text> Texts : K_SLOT_TEXTS;

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
    float2 glyphSize : XPE_GLYPH_SIZE;
    float2 glyphAtlasOffset : XPE_GLYPH_ATLAS_OFFSET;
    float4 textColor : XPE_TEXT_COLOR;
};

VSOut vs_main(VSIn vsIn)
{
    VSOut vsOut;

    float4 positionOut = float4(vsIn.positionLocal, 1.0f);

    Char c = Chars[vsIn.instanceIndex];
    Text text = Texts[c.TextIndex];

    float2 offsetScaled = float2(text.ModelMatrix[0][0], text.ModelMatrix[1][1]) * float2(c.Left, c.Top);
    float2 sizeScaled = float2(text.ModelMatrix[0][0], text.ModelMatrix[1][1]) * float2(c.Width, c.Height);

    float3 positionLocal = (vsIn.positionLocal * 0.5f) + 0.25f;

    positionOut = float4(positionLocal, 1.0f);
    positionOut *= float4(c.Width, c.Height, 1.0f, 1.0f);
    positionOut = mul(text.ModelMatrix, positionOut);

    positionOut.x += 0.5f * (offsetScaled.x + c.AdvanceX);
    positionOut.y -= 0.5f * ((sizeScaled.y - offsetScaled.y) + c.AdvanceY);

    positionOut = mul(text.Projection, positionOut);

    vsOut.positionOut = positionOut;
    vsOut.uv = vsIn.uv;
    vsOut.glyphSize = float2(c.Width, c.Height);
    vsOut.glyphAtlasOffset = float2(c.AtlasXOffset, c.AtlasYOffset);
    vsOut.textColor = text.Color;

    return vsOut;
}