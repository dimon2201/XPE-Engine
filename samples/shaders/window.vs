#include ../engine_shaders/types.shader
#include ../engine_shaders/batching.shader
#include ../engine_shaders/transforming.shader
#include ../engine_shaders/controls/camera.shader

struct VSIn
{
    float3 positionLocal : XPE_POSITION;
    float2 uv : XPE_UV;
    float3 normal : XPE_NORMAL; // normalized
    float3 tangent : XPE_TANGENT; // normalized
    uint instanceIndex : SV_InstanceID;
};

struct VSOut
{
    float3 positionWorld : XPE_POSITION_WORLD;
    float2 uv : XPE_UV2;
    float3 normal : XPE_NORMAL_WORLD;
    float4 positionClip : SV_POSITION;
    float3 viewPosition : XPE_VIEW_POSITION;
    uint materialIndex : XPE_MATERIAL_INDEX;
    float3x3 tbn : XPE_TBN;
};

VSOut vs_main(VSIn vsIn)
{
    VSOut vsOut = (VSOut)0;

    RenderInstance instance = Instances[vsIn.instanceIndex];
    float4x4 worldMatrix = Transforms[instance.TransformIndex].ModelMatrix;
    float4x4 worldNormalMatrix = Transforms[instance.TransformIndex].NormalMatrix;
    Camera camera = Cameras[instance.CameraIndex];

    float4 positionWorld = mul(worldMatrix, float4(vsIn.positionLocal, 1.0));
    float4 positionView  = mul(camera.View, positionWorld);
    float4 positionClip  = mul(camera.Projection, positionView);

    float3 normalWorld   = mul(worldNormalMatrix, float4(vsIn.normal, 1.0)).xyz;
    float3 tangentWorld  = mul(worldNormalMatrix, float4(vsIn.tangent, 1.0)).xyz;
    normalWorld = normalize(normalWorld);
    tangentWorld = normalize(tangentWorld);
    float3 bitangentWorld = -cross(normalWorld, tangentWorld);

    vsOut.positionWorld = positionWorld.xyz;
    vsOut.uv = vsIn.uv;
    vsOut.normal = normalWorld.xyz;
    vsOut.positionClip = positionClip;
    vsOut.viewPosition = camera.Position;
    vsOut.materialIndex = instance.MaterialIndex;
    vsOut.tbn = float3x3(tangentWorld, bitangentWorld, normalWorld);

    return vsOut;
}