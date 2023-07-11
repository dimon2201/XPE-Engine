struct Camera {
    float4x4 Projection;
    float4x4 View;
    float3 Position;
};

StructuredBuffer<Camera> Cameras : K_SLOT_CAMERAS;

float4x4 CameraViewProjection(uint index) {
    return mul(Cameras[index].Projection, Cameras[index].View);
}