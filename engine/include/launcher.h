#pragma once

#include <core/app.hpp>

using namespace xpe::core;

int main(int argc, char** argv) {

    // pull hardware config
    xpe::os::HardwareConfig::UpdateMemoryStats();
    xpe::os::HardwareConfig::UpdateVideoStats();

    // init memory pools
    MemoryPoolManager::Init();

    // run app implementation
    auto* application = CreateApplication();

    // read app configs
    if (!ReadJsonFile("config/config.json", application->Config))
    {
        FMT_ASSERT(false, "Failed to read app config from config/config.json file. Please provide config file!");
        return 0;
    }

    application->Run();

    delete application;

    // free memory pools
    MemoryPoolManager::Free();

    return 0;
}