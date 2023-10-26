#include ../types.shader
#include ../instancing.shader
#include ../transforming.shader
#include ../skeleton.shader

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
    float4 positionLight  : SV_POSITION;
};

VSOut vs_main(VSIn vsIn)
{
    VSOut vsOut = (VSOut)0;

    float4 positionTotal = float4(0, 0, 0, 0);
    float4 positionBone  = float4(vsIn.positionLocal, 1.0);
    int4 boneIds         = vsIn.boneIds;
    float4 boneWeights   = vsIn.boneWeights;
    uint bonesCount      = 0;
    uint bonesStride     = 0;
    Bones.GetDimensions(bonesCount, bonesStride);

    for (int i = 0 ; i < 4 ; i++)
    {
        int boneID = boneIds[i];
        float boneWeight = boneWeights[i];

        if (boneID == -1 || boneID >= bonesCount)
            continue;

        float4x4 boneTransform = Bones[boneID].Transform;

        positionTotal += mul(boneTransform, float4(vsIn.positionLocal, 1.0)) * boneWeight;
        positionBone = positionTotal;
    }

    RenderInstance instance     = Instances[vsIn.instanceIndex];
    float4x4 worldMatrix        = Transforms[instance.TransformIndex].ModelMatrix;
    float4x4 lightMatrix        = Transforms[instance.TransformIndex].LightMatrix;

    float4 positionWorld = mul(worldMatrix, positionBone);
    float4 positionLight = mul(lightMatrix, positionWorld);

    vsOut.positionLight = positionLight;

    return vsOut;
}