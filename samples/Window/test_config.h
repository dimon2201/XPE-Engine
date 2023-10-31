#pragma once

struct sTestConfig : public cJson
{
    JsonClass(
        sTestConfig,
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

struct sXmlConfig : public cXml
{
    XmlClass(sXmlConfig)

    bool AnimateLight = false;
    float CameraFar = 1000;
    float CameraMoveSpeed = 0.05f;
    float CameraZoomAcceleration = 10.0f;
    float CameraPanAcceleration = 5.0f;
    float CameraHorizontalSens = 0.005f;
    float CameraVerticalSens = 0.005f;

    void ToXml(xml &root) override {
        auto child = root.append_child("XmlConfig");
        child.append_attribute("Name").set_value(Name.c_str());
        child.append_attribute("AnimateLight").set_value(AnimateLight);
        child.append_attribute("CameraFar").set_value(CameraFar);
        child.append_attribute("CameraMoveSpeed").set_value(CameraMoveSpeed);
        child.append_attribute("CameraZoomAcceleration").set_value(CameraZoomAcceleration);
        child.append_attribute("CameraPanAcceleration").set_value(CameraPanAcceleration);
        child.append_attribute("CameraHorizontalSens").set_value(CameraHorizontalSens);
        child.append_attribute("CameraVerticalSens").set_value(CameraVerticalSens);
    }

    void FromXml(xml &root) override {
        root = root.child("XmlConfig");
        Name = root.attribute("Name").as_string();
        AnimateLight = root.attribute("AnimateLight").as_bool();
        CameraFar = root.attribute("CameraFar").as_float();
        CameraMoveSpeed = root.attribute("CameraMoveSpeed").as_float();
        CameraZoomAcceleration = root.attribute("CameraZoomAcceleration").as_float();
        CameraPanAcceleration = root.attribute("CameraPanAcceleration").as_float();
        CameraHorizontalSens = root.attribute("CameraHorizontalSens").as_float();
        CameraVerticalSens = root.attribute("CameraVerticalSens").as_float();
    }

};