#include ../types.shader
#include ../transforming.shader
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
    VSOut vsOut = (VSOut)0;

    Character character = Text[vsIn.instanceIndex];
    Transform transform = Transforms[character.TransformIndex];
    Camera camera = Cameras[character.CameraIndex];

    float2 offsetScaled = float2(transform.ModelMatrix[0][0], transform.ModelMatrix[1][1]) * float2(character.Left, character.Top);
    float2 sizeScaled = float2(transform.ModelMatrix[0][0], transform.ModelMatrix[1][1]) * float2(character.Width, character.Height);

    float3 positionLocal = (vsIn.positionLocal * 0.5f) + 0.25f;

    float4 positionWorld = float4(positionLocal, 1.0f);
    positionWorld *= float4(character.Width, character.Height, 1.0f, 1.0f);
    positionWorld = mul(transform.ModelMatrix, positionWorld);

    positionWorld.x += 0.5f * (offsetScaled.x + character.AdvanceX);
    positionWorld.y -= 0.5f * ((sizeScaled.y - offsetScaled.y) + character.AdvanceY);

    float4 positionView = mul(camera.View, positionWorld);
    float4 positionClip = mul(camera.Projection, positionView);

    vsOut.viewPosition = camera.Position;
    vsOut.texcoord = vsIn.texcoord;
    vsOut.normal = normalize(vsIn.normal);
    vsOut.positionClip = positionClip;
    vsOut.glyphSize = float2(character.Width, character.Height);
    vsOut.glyphAtlasOffset = float2(character.AtlasXOffset, character.AtlasYOffset);

    return vsOut;
}