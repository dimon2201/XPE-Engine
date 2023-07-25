#pragma once

#include <json/json.h>

struct TestConfig final {
    bool AnimateLight = false;
    float CameraFar = 1000;
    float CameraMoveSpeed = 0.05f;
    float CameraZoomAcceleration = 10.0f;
    float CameraPanAcceleration = 5.0f;
    float CameraHorizontalSens = 0.005f;
    float CameraVerticalSens = 0.005f;
};

class TestConfigReader final {

public:
    static TestConfig Read(const char* filepath);

private:
    static void Parse(const std::string& source, TestConfig& config);

};
