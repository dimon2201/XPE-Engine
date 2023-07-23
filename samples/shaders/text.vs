#include ../engine_shaders/types.shader
#include ../engine_shaders/text/types.shader
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
    uint materialIndex : XPE_MATERIAL_INDEX;
};

VSOut vs_main(VSIn vsIn)
{
    VSOut vsOut = (VSOut)0;

    TextGlyphInstance instance = TextGlyphs[vsIn.instanceIndex];
    Transform transform = Transforms[instance.TransformIndex];
    Camera camera = Cameras[instance.CameraIndex];

    float4 positionWorld = mul(transform.Matrix, float4(vsIn.positionLocal, 1.0));
    float4 positionView = mul(camera.View, positionWorld);
    float4 positionClip = mul(camera.Projection, positionView);

    vsOut.positionWorld = positionWorld.xyz;
    vsOut.viewPosition = camera.Position;
    vsOut.texcoord = vsIn.texcoord;
    vsOut.normal = normalize(vsIn.normal);
    vsOut.positionClip = positionClip;
    vsOut.materialIndex = 0;

    return vsOut;
}