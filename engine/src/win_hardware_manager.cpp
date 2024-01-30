#ifdef WINDOWS

#define _WIN32_WINNT 0x0501
#include <Windows.h>
#include "hardware_manager.hpp"


namespace xpe
{
    namespace core
    {
        void MHardware::UpdateMemoryStats()
        {
            MEMORYSTATUSEX memStatus;
            memStatus.dwLength = sizeof(memStatus);
            GlobalMemoryStatusEx(&memStatus);
            Memory.TotalPhysical = memStatus.ullTotalPhys;
            Memory.AvailablePhysical = memStatus.ullAvailPhys;
            Memory.TotalPageFile = memStatus.ullTotalPageFile;
            Memory.AvailablePageFile = memStatus.ullAvailPageFile;
            Memory.TotalVirtual = memStatus.ullTotalVirtual;
            Memory.AvailableVirtual = memStatus.ullAvailVirtual;
            Memory.AvailableVirtualExtended = memStatus.ullAvailExtendedVirtual;
        }

        void MHardware::UpdateCpuStats()
        {
            SYSTEM_INFO sysinfo;
            GetSystemInfo(&sysinfo);
            CPU.Threads = sysinfo.dwNumberOfProcessors;
            CPU.Cores = sysinfo.dwNumberOfProcessors / 2;
        }

        u32 MHardware::GetCurrentPID()
        {
            return GetCurrentProcessId();
        }
    }
}

#endif // WINDOWS