#include ../types.shader
#include ../instancing.shader
#include ../skeleton.shader
#include ../lighting.shader
#include ../shadow.shader

struct VSIn
{
    float3 positionLocal : XPE_POSITION;
    int4 boneIds         : XPE_BONE_IDS; // boneId = -1 - bone absent
    float4 boneWeights   : XPE_BONE_WEIGHTS;
    uint instanceIndex   : SV_InstanceID;
};

float4 vs_main(VSIn vsIn) : SV_POSITION
{
    RenderInstance instance = Instances[vsIn.instanceIndex];

    float4 positionWorld = mul(instance.ModelMatrix, float4(vsIn.positionLocal.xyz, 1.0));
    float4 positionView = mul(DirectLights[instance.LightIndex].View, float4(positionWorld.xyz, 1.0));
    float4 positionClip = mul(DirectLights[instance.LightIndex].Projection, float4(positionView.xyz, 1.0));

    positionClip.z = positionView.z / 1024.0;
    positionClip.w = 1.0;

    return positionClip;
}
