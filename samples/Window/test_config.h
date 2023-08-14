#pragma once

struct TestConfig : xpe::res::JsonObject
{
    JsonClass(
        TestConfig,
        AnimateLight,
        CameraFar,
        CameraMoveSpeed,
        CameraZoomAcceleration,
        CameraPanAcceleration,
        CameraHorizontalSens,
        CameraVerticalSens
    )

    bool AnimateLight = false;
    float CameraFar = 1000;
    float CameraMoveSpeed = 0.05f;
    float CameraZoomAcceleration = 10.0f;
    float CameraPanAcceleration = 5.0f;
    float CameraHorizontalSens = 0.005f;
    float CameraVerticalSens = 0.005f;
};