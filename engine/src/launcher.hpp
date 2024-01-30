#pragma once

#include <app.hpp>

int main(int argc, char** argv)
{
    MHardware::UpdateMemoryStats();
    MHardware::UpdateCpuStats();
    MProfile::BeginSession();
    MMemory::Init();
    auto* application = CreateApplication();
    application->Run();
    delete application;
    MMemory::Free();
    MProfile::EndSession();
    return 0;
}