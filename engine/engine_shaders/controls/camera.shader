struct CameraBufferData {
    float4x4 Projection;
    float4x4 View;
    float3 Position;
};

StructuredBuffer<CameraBufferData> Cameras : K_SLOT_CAMERAS;

float4x4 CameraViewProjection() {
    return mul(Cameras[0].Projection, Cameras[0].View);
}