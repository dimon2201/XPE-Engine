#include ../engine_shaders/types.shader
#include ../engine_shaders/batching.shader
#include ../engine_shaders/transforming.shader
#include ../engine_shaders/controls/camera.shader

struct VSIn
{
    float3 positionLocal : XPE_POSITION_LOCAL;
    float2 texcoord : XPE_TEXCOORD;
    float3 normal : XPE_NORMAL;
    uint instanceIndex : SV_InstanceID;
};

struct VSOut
{
    float3 positionWorld : XPE_POSITION_WORLD;
    float2 texcoord : XPE_TEXCOORD2;
    float3 normal : XPE_NORMAL2;
    float4 positionClip : SV_POSITION;
    float3 viewPosition : XPE_VIEW_POSITION;
    uint materialIndex : XPE_MATERIAL_INDEX;
};

VSOut vs_main(VSIn vsIn)
{
    VSOut vsOut = (VSOut)0;
    RenderInstance instance = Instances[vsIn.instanceIndex];
    Transform instanceTransform = Transforms[instance.TransformIndex];
    Camera instanceCamera = Cameras[instance.CameraIndex];

    float4 positionWorld = mul(instanceTransform.Matrix, float4(vsIn.positionLocal, 1.0));
    vsOut.positionWorld = positionWorld.xyz;
    vsOut.viewPosition = instanceCamera.Position;
    vsOut.texcoord = vsIn.texcoord;
    vsOut.normal = vsIn.normal;
    vsOut.positionClip = mul(CameraViewProjection(instance.CameraIndex), float4(vsOut.positionWorld, 1.0));
    vsOut.materialIndex = instance.MaterialIndex;

    return vsOut;
}