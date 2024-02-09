#pragma once

#include <app.hpp>

int main(int argc, char** argv)
{
    MHardware::UpdateMemInfo();
    MHardware::UpdateCpuInfo();
    MProfile::BeginSession();
    MMemory::Init();
    auto* application = CreateApplication();
    application->Run();
    delete application;
    MMemory::Free();
    MProfile::EndSession();
    return 0;
}