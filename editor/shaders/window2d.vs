#include ../engine_shaders/types.shader
#include ../engine_shaders/batching.shader
#include ../engine_shaders/transforming.shader
#include ../engine_shaders/controls/camera.shader

struct VSIn
{
    float2 positionLocal : XPE_POSITION_2D;
    float2 uv : XPE_UV;
    uint instanceIndex : SV_InstanceID;
};

struct VSOut
{
    float2 positionWorld : XPE_POSITION_WORLD;
    float2 uv : XPE_UV2;
    float4 positionClip : SV_POSITION;
    float2 viewPosition : XPE_VIEW_POSITION;
};

VSOut vs_main(VSIn vsIn)
{
    VSOut vsOut = (VSOut)0;
    RenderInstance2D instance = Instances2D[vsIn.instanceIndex];
    float3x3 worldMatrix = Transforms2D[instance.TransformIndex].ModelMatrix;
    Camera camera = Cameras[instance.CameraIndex];

    float4 positionWorld = float4(mul(worldMatrix, float3(vsIn.positionLocal, 1.0)), 1.0);
    float4 positionView = mul(camera.View, positionWorld);
    positionView.z = 1.0;
    positionView.w = 1.0;
    float4 positionClip = mul(camera.Projection, positionView);

    vsOut.positionWorld = positionWorld.xy;
    vsOut.viewPosition = camera.Position.xy;
    vsOut.uv = vsIn.uv;
    vsOut.positionClip = positionClip;

    return vsOut;
}