cbuffer CameraBuffer : K_SLOT_CAMERA
{
    float3 CameraPosition;
    float4x4 View;
    float4x4 Projection;
    float TopLeftX;
    float TopLeftY;
    float Width;
    float Height;
    float MinDepth;
    float MaxDepth;
    float Exposure;
    float Gamma;
};