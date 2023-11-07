#include ../types.shader
#include ../camera.shader
#include ../text/text.shader

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
    float2 glyphSize : XPE_GLYPH_SIZE;
    float2 glyphAtlasOffset : XPE_GLYPH_ATLAS_OFFSET;
};

VSOut vs_main(VSIn vsIn)
{
    VSOut vsOut;

    Character character = Text[vsIn.instanceIndex];

    float2 offsetScaled = float2(TextModelMatrix[0][0], TextModelMatrix[1][1]) * float2(character.Left, character.Top);
    float2 sizeScaled = float2(TextModelMatrix[0][0], TextModelMatrix[1][1]) * float2(character.Width, character.Height);

    float3 positionLocal = (vsIn.positionLocal * 0.5f) + 0.25f;

    float4 positionNDC = float4(positionLocal, 1.0f);
    positionNDC *= float4(character.Width, character.Height, 1.0f, 1.0f);
    positionNDC = mul(TextModelMatrix, positionNDC);

    positionNDC.x += 0.5f * (offsetScaled.x + character.AdvanceX);
    positionNDC.y -= 0.5f * ((sizeScaled.y - offsetScaled.y) + character.AdvanceY);

    positionNDC.xy /= float2(ViewportWidth, ViewportHeight);
    positionNDC.xy = (positionNDC.xy * 2.0f) - 1.0f;

    vsOut.uv = vsIn.uv;
    vsOut.positionNDC = positionNDC;
    vsOut.glyphSize = float2(character.Width, character.Height);
    vsOut.glyphAtlasOffset = float2(character.AtlasXOffset, character.AtlasYOffset);

    return vsOut;
}