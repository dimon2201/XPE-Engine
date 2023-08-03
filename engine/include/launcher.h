#pragma once

#include <core/app.hpp>

using namespace xpe::core;

int main(int argc, char** argv) {

    // read app configs
    if (!ReadJsonFile("config/config.json", AppConfig::Get()))
    {
        FMT_ASSERT(false, "Failed to read app config from config/config.json file. Please provide config file!");
        return 0;
    }

    // read memory configs
    if (!ReadJsonFile("config/memory_config.json", MemoryConfig::Get()))
    {
        FMT_ASSERT(false, "Failed to read memory config from config/memory_config.json file. Please provide config file!");
        return 0;
    }

    // init memory pools
    MemoryPoolManager::Init(MemoryConfig::Get());

    // run app implementation
    auto* application = CreateApplication();
    application->Run();
    delete application;

    // free memory pools
    MemoryPoolManager::Free();

    return 0;
}