#pragma once

#include <core/app.hpp>

using namespace xpe::core;

int main(int argc, char** argv) {
    // read app configs
    AppConfig::Get() = AppConfigReader::Read("config.json");
    // run app implementation
    auto* application = CreateApplication();
    application->Run();
    delete application;

    return 0;
}