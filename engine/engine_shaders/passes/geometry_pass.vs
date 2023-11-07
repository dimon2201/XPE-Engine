#include ../types.shader
#include ../instancing.shader
#include ../camera.shader
#include ../skeleton.shader
#include ../lighting_matrices.shader

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
    float3 positionWorld : XPE_POSITION_WORLD;
    float zView          : XPE_Z_VIEW;
    float2 uv            : XPE_UV2;
    float3 normal        : XPE_NORMAL_WORLD;
    float4 positionClip  : SV_POSITION;
    float4 positionLight : XPE_POSITION_LIGHT;
    float3 viewPosition  : XPE_VIEW_POSITION;
    uint materialIndex   : XPE_MATERIAL_INDEX;
    float3x3 tbn         : XPE_TBN;
    float gamma          : XPE_GAMMA;
};

VSOut vs_main(VSIn vsIn)
{
    VSOut vsOut = (VSOut)0;

    float4 positionTotal = float4(0, 0, 0, 0);
    float4 positionBone  = float4(vsIn.positionLocal, 1.0);
    float3 normalTotal   = float3(0, 0, 0);
    float3 normalBone    = vsIn.normal;
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
        normalTotal += mul(boneTransform, float4(vsIn.normal, 1.0)).xyz;
        normalBone = normalTotal;
    }

    float4x4 worldMatrix        = instance.ModelMatrix;
    float4x4 worldNormalMatrix  = instance.NormalMatrix;
    float4x4 lightMatrix        = DirectLightMatrices[0].Matrix;

    float4 positionWorld = mul(worldMatrix, positionBone);
    float4 positionView  = mul(CameraView, positionWorld);
    float4 positionClip  = mul(CameraProjection, positionView);
    float4 positionLight = mul(lightMatrix, positionWorld);

    float3 normalWorld   = mul(worldNormalMatrix, float4(normalBone, 1.0)).xyz;
    float3 tangentWorld  = mul(worldNormalMatrix, float4(vsIn.tangent, 1.0)).xyz;
    normalWorld = normalize(normalWorld);
    tangentWorld = normalize(tangentWorld);
    float3 bitangentWorld = -cross(normalWorld, tangentWorld);

    vsOut.positionWorld = positionWorld.xyz;
    vsOut.zView         = positionView.z;
    vsOut.uv            = vsIn.uv;
    vsOut.normal        = normalWorld.xyz;
    vsOut.positionClip  = positionClip;
    vsOut.positionLight = positionLight;
    vsOut.viewPosition  = CameraPosition;
    vsOut.materialIndex = instance.MaterialIndex;
    vsOut.tbn           = float3x3(tangentWorld, bitangentWorld, normalWorld);
    vsOut.gamma         = ViewportGamma;

    return vsOut;
}