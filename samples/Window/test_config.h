#pragma once

#include <json/json.h>

struct TestConfig final {
    bool AnimateLight = false;
    float CameraFar = 1000;
    float CameraMoveSpeed = 1.0f;
    float CameraZoomAcceleration = 1.0f;
    float CameraPanAcceleration = 1.0f;
    float CameraHorizontalSens = 0.5f;
    float CameraVerticalSens = 0.5f;
};

class TestConfigReader final {

public:
    static TestConfig Read(const char* filepath);

private:
    static void Parse(const std::string& source, TestConfig& config);

};
