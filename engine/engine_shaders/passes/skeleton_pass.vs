#include ../types.shader
#include ../instancing.shader
#include ../transforming.shader
#include ../camera.shader
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
    float3 positionWorld : XPE_POSITION_WORLD;
    float zView          : XPE_Z_VIEW;
    float2 uv            : XPE_UV2;
    float3 normal        : XPE_NORMAL_WORLD;
    float4 positionClip  : SV_POSITION;
    float3 viewPosition  : XPE_VIEW_POSITION;
    uint materialIndex   : XPE_MATERIAL_INDEX;
    float3x3 tbn         : XPE_TBN;
    float3 shadowCoords  : XPE_SHADOW_COORDS;
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
        normalTotal += mul(boneTransform, vsIn.normal);
        normalBone = normalTotal;
    }

    RenderInstance instance     = Instances[vsIn.instanceIndex];
    float4x4 worldMatrix        = Transforms[instance.TransformIndex].ModelMatrix;
    float4x4 worldNormalMatrix  = Transforms[instance.TransformIndex].NormalMatrix;
    float4x4 lightMatrix        = Transforms[instance.TransformIndex].LightMatrix;
    Camera camera               = Cameras[0];

    float4 positionWorld = mul(worldMatrix, positionBone);
    float4 positionView  = mul(camera.View, positionWorld);
    float4 positionClip  = mul(camera.Projection, positionView);
    float4 positionLight = mul(lightMatrix, positionWorld);
    float3 shadowCoords  = positionLight.xyz / positionLight.w;
    shadowCoords         = shadowCoords * 0.5 + 0.5;

    float3 normalWorld   = mul(worldNormalMatrix, normalBone).xyz;
    float3 tangentWorld  = mul(worldNormalMatrix, float4(vsIn.tangent, 1.0)).xyz;
    normalWorld = normalize(normalWorld);
    tangentWorld = normalize(tangentWorld);
    float3 bitangentWorld = -cross(normalWorld, tangentWorld);

    vsOut.positionWorld = positionWorld.xyz;
    vsOut.zView         = positionView.z;
    vsOut.uv            = vsIn.uv;
    vsOut.normal        = normalWorld.xyz;
    vsOut.positionClip  = positionClip;
    vsOut.viewPosition  = camera.Position;
    vsOut.materialIndex = instance.MaterialIndex;
    vsOut.tbn           = float3x3(tangentWorld, bitangentWorld, normalWorld);
    vsOut.shadowCoords  = shadowCoords;

    return vsOut;
}