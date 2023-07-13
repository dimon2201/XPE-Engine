#pragma once

#include <json/json.h>

struct TestConfig final {
    bool AnimateLight = false;
};

class TestConfigReader final {

public:
    static TestConfig Read(const char* filepath);

private:
    static void Parse(const std::string& source, TestConfig& config);

};
