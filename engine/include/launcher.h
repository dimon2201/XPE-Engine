#pragma once

#include <core/app.hpp>

using namespace xpe::core;

int main(int argc, char** argv) {

    // pull hardware config
    xpe::os::Hardware::UpdateMemoryStats();
    xpe::os::Hardware::UpdateVideoStats();

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