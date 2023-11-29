#pragma once

struct sTestConfig : public cJson
{
    bool AnimateLight = false;
    float CameraFar = 1000;
    float CameraMoveSpeed = 0.05f;
    float CameraZoomAcceleration = 10.0f;
    float CameraPanAcceleration = 5.0f;
    float CameraHorizontalSens = 0.005f;
    float CameraVerticalSens = 0.005f;

    JSON_CLASS(
        sTestConfig,
        AnimateLight,
        CameraFar,
        CameraMoveSpeed,
        CameraZoomAcceleration,
        CameraPanAcceleration,
        CameraHorizontalSens,
        CameraVerticalSens
    )
};

struct sXmlConfig : public cXml
{
    bool AnimateLight = false;
    float CameraFar = 1000;
    float CameraMoveSpeed = 0.05f;
    float CameraZoomAcceleration = 10.0f;
    float CameraPanAcceleration = 5.0f;
    float CameraHorizontalSens = 0.005f;
    float CameraVerticalSens = 0.005f;

    XML_CLASS(
        "XmlConfig",
        AnimateLight,
        CameraFar,
        CameraMoveSpeed,
        CameraZoomAcceleration,
        CameraPanAcceleration,
        CameraHorizontalSens,
        CameraVerticalSens
    )
};