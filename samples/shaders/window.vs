#include ../engine_shaders/slots.shader
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
    uint materialIndex : XPE_MATERIAL_INDEX;
};

struct RenderInstance
{
    float4 Position;
    uint MaterialIndex;
};

StructuredBuffer<RenderInstance> Instances : K_SLOT_INSTANCES;

VSOut vs_main(VSIn vsIn)
{
    VSOut vsOut = (VSOut)0;
    RenderInstance instance = Instances[vsIn.instanceIndex];
    vsOut.positionWorld = 0.5 * vsIn.positionLocal + instance.Position.xyz;
    vsOut.texcoord = vsIn.texcoord;
    vsOut.normal = vsIn.normal;
    vsOut.positionClip = mul(CameraViewProjection(), float4(vsOut.positionWorld, 1.0));
    vsOut.materialIndex = instance.MaterialIndex;
    return vsOut;
}