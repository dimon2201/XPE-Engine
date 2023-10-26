struct Camera {
    float3 Position;
    float4x4 View;
    float4x4 Projection;
};

StructuredBuffer<Camera> Cameras : K_SLOT_CAMERAS;

float4x4 CameraViewProjection() {
    return mul(Cameras[0].Projection, Cameras[0].View);
}