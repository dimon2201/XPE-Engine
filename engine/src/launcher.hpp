#pragma once

#include <core/app.hpp>

using namespace xpe::core;

int main(int argc, char** argv) {
    // pull OS hardware params
    xpe::core::cHardwareManager::UpdateMemoryStats();
    xpe::core::cHardwareManager::UpdateCpuStats();

    // init memory pools
    cMemoryManager::Init();

    // run app implementation
    auto* application = CreateApplication();

    application->Run();

    delete application;

    // free memory pools
    cMemoryManager::Free();

    return 0;
}