cbuffer CameraBuffer : K_SLOT_CAMERA
{
    float3 CameraPosition;
    float4x4 CameraView;
    float4x4 CameraProjection;
    float ViewportTopLeftX;
    float ViewportTopLeftY;
    float ViewportWidth;
    float ViewportHeight;
    float ViewportMinDepth;
    float ViewportMaxDepth;
    float CameraExposure;
    float ViewportGamma;
};