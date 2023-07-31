#include ../engine_shaders/types.shader
#include ../engine_shaders/batching.shader
#include ../engine_shaders/transforming.shader
#include ../engine_shaders/controls/camera.shader

struct VSIn
{
    float3 positionLocal : XPE_POSITION;
    float2 texcoord : XPE_UV;
    float3 normal : XPE_NORMAL;
    uint instanceIndex : SV_InstanceID;
};

struct VSOut
{
    float3 positionWorld : XPE_POSITION_WORLD;
    float2 texcoord : XPE_UV2;
    float3 normal : XPE_NORMAL2;
    float4 positionClip : SV_POSITION;
    float3 viewPosition : XPE_VIEW_POSITION;
    float2 glyphSize : XPE_GLYPH_SIZE;
    float3 glyphData : XPE_GLYPH_DATA;
    float2 glyphAtlasOffset : XPE_GLYPH_ATLAS_OFFSET;
};

VSOut vs_main(VSIn vsIn)
{
    VSOut vsOut = (VSOut)0;

    TextGlyphInstance instance = TextGlyphs[vsIn.instanceIndex];
    Transform transform = Transforms[instance.TransformIndex];
    Camera camera = Cameras[instance.CameraIndex];

    float3 positionLocal = (vsIn.positionLocal * 0.5f) + 0.25f;
    float4 positionWorld = float4(positionLocal, 1.0f);
    //positionWorld = mul(transform.ModelMatrix, positionWorld);
    positionWorld.x *= instance.Width;
    positionWorld.y *= instance.Height;
    positionWorld = mul(transform.ModelMatrix, positionWorld);
    positionWorld.x += 0.5f * ((instance.Left / instance.GlyphSize) + instance.Advance);
    positionWorld.y -= 0.5f * ((instance.Height - instance.Top) / instance.GlyphSize);
    float4 positionView = positionWorld + float4(0.0f, 0.0f, 8.0f, 0.0f);
    float4 positionClip = mul(camera.Projection, positionView);

    vsOut.positionWorld = positionWorld.xyz;
    vsOut.viewPosition = camera.Position;
    vsOut.texcoord = vsIn.texcoord;
    vsOut.normal = normalize(vsIn.normal);
    vsOut.positionClip = positionClip;
    vsOut.glyphSize = float2(instance.Width, instance.Height);
    vsOut.glyphData = float3(instance.Left, instance.Top, instance.Advance);
    vsOut.glyphAtlasOffset = float2(instance.AtlasXOffset, instance.AtlasYOffset);

    return vsOut;
}