#pragma once

#include <core/app.hpp>

using namespace xpe::core;

int main(int argc, char** argv) {
    // pull OS hardware params
    xpe::core::Hardware::UpdateMemoryStats();
    xpe::core::Hardware::UpdateCpuStats();

    // init memory pools
    Memory::Init();

    // run app implementation
    auto* application = CreateApplication();

    application->Run();

    delete application;

    // free memory pools
    Memory::Free();

    return 0;
}