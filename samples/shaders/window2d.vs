#include ../engine_shaders/types.shader
#include ../engine_shaders/batching.shader
#include ../engine_shaders/transforming.shader
#include ../engine_shaders/controls/camera.shader

struct VSIn
{
    float2 positionLocal : XPE_POSITION_2D;
    float2 texcoord : XPE_UV;
    uint instanceIndex : SV_InstanceID;
};

struct VSOut
{
    float2 positionWorld : XPE_POSITION_WORLD;
    float2 texcoord : XPE_UV2;
    float4 positionClip : SV_POSITION;
    float2 viewPosition : XPE_VIEW_POSITION;
};

VSOut vs_main(VSIn vsIn)
{
    VSOut vsOut = (VSOut)0;
    RenderInstance2D instance = Instances2D[vsIn.instanceIndex];
    Transform2D instanceTransform = Transforms2D[instance.TransformIndex];
    Camera instanceCamera = Cameras[instance.CameraIndex];

    float4 positionWorld = float4(mul(instanceTransform.Matrix, float3(vsIn.positionLocal, 1.0)), 1.0);
    vsOut.positionWorld = positionWorld.xy;
    vsOut.viewPosition = instanceCamera.Position.xy;
    vsOut.texcoord = vsIn.texcoord;
    vsOut.positionClip = float4(vsIn.positionLocal, 0.0, 1.0);

    return vsOut;
}