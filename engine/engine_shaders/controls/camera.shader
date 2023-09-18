struct Camera {
    float3 Position;
    float4x4 View;
    float4x4 Projection;
};

StructuredBuffer<Camera> Cameras : K_SLOT_CAMERAS;

Camera MainCamera() {
    return Cameras[0];
}

float4x4 CameraViewProjection(uint index) {
    return mul(Cameras[index].Projection, Cameras[index].View);
}