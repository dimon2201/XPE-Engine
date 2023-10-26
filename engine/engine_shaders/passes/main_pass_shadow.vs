#include ../types.shader
#include ../instancing.shader
#include ../transforming.shader

struct VSIn
{
    float3 positionLocal : XPE_POSITION;
    float2 uv            : XPE_UV;
    float3 normal        : XPE_NORMAL; // normalized
    float3 tangent       : XPE_TANGENT; // normalized
    int4 boneIds         : XPE_BONE_IDS; // boneId = -1 - bone absent
    float4 boneWeights   : XPE_BONE_WEIGHTS;
    uint instanceIndex   : SV_InstanceID;
};

struct VSOut
{
    float4 positionLight : SV_POSITION;
};

VSOut vs_main(VSIn vsIn)
{
    VSOut vsOut;

    RenderInstance instance = Instances[vsIn.instanceIndex];
    float4x4 worldMatrix    = Transforms[instance.TransformIndex].ModelMatrix;
    float4x4 lightMatrix    = Transforms[instance.TransformIndex].LightMatrix;

    float4 positionWorld    = mul(worldMatrix, float4(vsIn.positionLocal, 1.0));
    float4 positionLight    = mul(lightMatrix, positionWorld);

    vsOut.positionLight = positionLight;

    return vsOut;
}