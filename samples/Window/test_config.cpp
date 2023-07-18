#include "test_config.h"

using namespace xpe::core;

TestConfig TestConfigReader::Read(const char *filepath) {
    TestConfig config;

    std::ifstream file(filepath, std::ios::in | std::ios::binary);

    if (!file.is_open()) {
        FMT_ASSERT(false, "Please specify correct path to test_config.json file!");
        return config;
    }

    file.seekg(0, std::ios::end);
    usize fileByteSize = file.tellg();
    char* str = (char*)malloc(fileByteSize);
    memset(str, 0, fileByteSize);
    file.seekg(0, std::ios::beg);
    file.read(str, fileByteSize);
    file.close();

    Parse(std::string(str), config);

    free(str);

    return config;
}

void TestConfigReader::Parse(const std::string &source, TestConfig &config) {
    Json::Value root;
    Json::Reader reader;
    reader.parse(source, root);

    config.AnimateLight = root["animate_light"].asBool();

    config.CameraFar = root["camera_far"].asFloat();
    config.CameraMoveSpeed = root["camera_move_speed"].asFloat();
    config.CameraZoomAcceleration = root["camera_zoom_acceleration"].asFloat();
    config.CameraPanAcceleration = root["camera_pan_acceleration"].asFloat();
    config.CameraHorizontalSens = root["camera_horizontal_sens"].asFloat();
    config.CameraVerticalSens = root["camera_vertical_sens"].asFloat();

}
