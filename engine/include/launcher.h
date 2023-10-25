#pragma once

#include <core/app.hpp>

using namespace xpe::core;

int main(int argc, char** argv) {
    // pull OS hardware params
    xpe::core::HardwareManager::UpdateMemoryStats();
    xpe::core::HardwareManager::UpdateCpuStats();

    // init memory pools
    MemoryManager::Init();

    // run app implementation
    auto* application = CreateApplication();

    application->Run();

    delete application;

    // free memory pools
    MemoryManager::Free();

    return 0;
}