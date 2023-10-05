#pragma once

#include <core/app.hpp>

using namespace xpe::core;

int main(int argc, char** argv) {

    // pull hardware config
    xpe::core::Hardware::UpdateMemoryStats();
    xpe::core::Hardware::UpdateVideoStats();
    xpe::core::Hardware::UpdateCpuStats();

    // init memory pools
    MemoryPoolManager::Init();

    // run app implementation
    auto* application = CreateApplication();

    application->Run();

    delete application;

    // free memory pools
    MemoryPoolManager::Free();

    return 0;
}