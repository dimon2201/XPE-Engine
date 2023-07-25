#pragma once

#include <core/app.hpp>

using namespace xpe::core;

int main(int argc, char** argv) {
    // read app configs
    AppConfig::Get() = AppConfigReader::Read("config/config.json");

    // read memory configs
    MemoryConfig::Get() = MemoryConfigReader::Read("config/memory_config.json");

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