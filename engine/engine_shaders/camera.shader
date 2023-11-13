cbuffer CameraBuffer : K_SLOT_CAMERA
{
    float3 CameraPosition;
    float4x4 CameraView;
    float4x4 CameraProjection;
    float ViewportLeft;
    float ViewportTop;
    float ViewportWidth;
    float ViewportHeight;
    float ViewportMinDepth;
    float ViewportMaxDepth;
    float CameraExposure;
    float ViewportGamma;
};