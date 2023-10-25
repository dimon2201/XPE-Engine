#include ../types.shader
#include ../instance.shader
#include ../transforming.shader

struct VSIn
{
    float3 positionLocal : XPE_POSITION;
    uint instanceIndex : SV_InstanceID;
};

struct VSOut
{
    float4 positionDLS : SV_POSITION;
};

VSOut vs_main(VSIn vsIn)
{
    VSOut vsOut;

    RenderInstance instance = Instances[vsIn.instanceIndex];
    float4x4 worldMatrix    = Transforms[instance.TransformIndex].ModelMatrix;
    // float4x4 dlsMatrix      = Transforms[instance.TransformIndex].DLSMatrix;

    float4 positionWorld    = mul(worldMatrix, float4(vsIn.positionLocal, 1.0));
    // float4 positionDLS      = mul(dlsMatrix, positionWorld);
    float4 positionDLS      = float4(0, 0, 0, 0);

    vsOut.positionDLS       = positionDLS;

    return vsOut;
}