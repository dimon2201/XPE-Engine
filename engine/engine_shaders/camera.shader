struct CameraBufferData {
    float4x4 Projection;
    float4x4 View;
    float3 Position;
};

cbuffer CameraBuffer : register(b0)
{
    CameraBufferData Camera;
};

float4x4 CameraViewProjection() {
    return mul(Camera.Projection, Camera.View);
}