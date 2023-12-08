#include ../types.shader
#include ../instancing.shader
#include ../skeleton.shader
#include ../lighting.shader

struct VSIn
{
    float3 positionLocal : XPE_POSITION;
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
    VSOut vsOut;

    float4 positionTotal = float4(0, 0, 0, 0);
    float4 positionBone  = float4(vsIn.positionLocal, 1.0);
    int4 boneIds         = vsIn.boneIds;
    float4 boneWeights   = vsIn.boneWeights;
    RenderInstance instance = Instances[vsIn.instanceIndex];
    uint skeletonIndex   = instance.SkeletonIndex;

    for (int i = 0 ; i < 4 ; i++)
    {
        int boneID = boneIds[i];
        float boneWeight = boneWeights[i];

        if (boneID == -1)
            continue;

        float4x4 boneTransform = Skeletons[boneID + skeletonIndex].Transform;

        positionTotal += mul(boneTransform, float4(vsIn.positionLocal, 1.0)) * boneWeight;
        positionBone = positionTotal;
    }

    float4x4 worldMatrix = instance.ModelMatrix;
    float4x4 lightMatrix = DirectLights[instance.LightIndex].ViewProjection;

    float4 positionWorld = mul(worldMatrix, float4(positionBone.xyz, 1.0));
    float4 positionLight = mul(lightMatrix, float4(positionWorld.xyz, 1.0));

    vsOut.positionLight = positionLight;

    return vsOut;
}