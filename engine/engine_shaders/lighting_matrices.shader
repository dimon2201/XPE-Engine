struct DirectLightMatrix
{
    float4x4 Matrix;
};

StructuredBuffer<DirectLightMatrix> DirectLightMatrices : K_SLOT_DIRECT_LIGHT_MATRICES;