cbuffer CameraBuffer : register(b0)
{
    float4x4 CameraProjection;
    float4x4 CameraView;
    float3 CameraPosition;
};

float4x4 CameraViewProjection() {
    return mul(CameraProjection, CameraView);
}