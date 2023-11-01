#include ../types.shader
#include ../camera.shader
#include ../text/text.shader

struct VSIn
{
    float3 positionLocal : XPE_POSITION;
    float2 texcoord : XPE_UV;
    float3 normal : XPE_NORMAL;
    uint instanceIndex : SV_InstanceID;
};

struct VSOut
{
    float2 texcoord : XPE_UV2;
    float3 normal : XPE_NORMAL2;
    float4 positionClip : SV_POSITION;
    float3 viewPosition : XPE_VIEW_POSITION;
    float2 glyphSize : XPE_GLYPH_SIZE;
    float2 glyphAtlasOffset : XPE_GLYPH_ATLAS_OFFSET;
};

VSOut vs_main(VSIn vsIn)
{
    VSOut vsOut;

    Character character = Text[vsIn.instanceIndex];

    float2 offsetScaled = float2(character.ModelMatrix[0][0], character.ModelMatrix[1][1]) * float2(character.Left, character.Top);
    float2 sizeScaled = float2(character.ModelMatrix[0][0], character.ModelMatrix[1][1]) * float2(character.Width, character.Height);

    float3 positionLocal = (vsIn.positionLocal * 0.5f) + 0.25f;

    float4 positionNDC = float4(positionLocal, 1.0f);
    positionNDC *= float4(character.Width, character.Height, 1.0f, 1.0f);
    positionNDC = mul(character.ModelMatrix, positionNDC);

    positionNDC.x += 0.5f * (offsetScaled.x + character.AdvanceX);
    positionNDC.y -= 0.5f * ((sizeScaled.y - offsetScaled.y) + character.AdvanceY);

    positionNDC.xy /= float2(Width, Height);
    positionNDC.xy = (positionNDC.xy * 2.0f) - 1.0f;

    vsOut.viewPosition = CameraPosition;
    vsOut.texcoord = vsIn.texcoord;
    vsOut.normal = normalize(vsIn.normal);
    vsOut.positionClip = positionNDC;
    vsOut.glyphSize = float2(character.Width, character.Height);
    vsOut.glyphAtlasOffset = float2(character.AtlasXOffset, character.AtlasYOffset);

    return vsOut;
}