#ifdef WINDOWS

#define _WIN32_WINNT 0x0501
#include <Windows.h>
#include <hardware_manager.hpp>

namespace xpe
{
    namespace core
    {
        void MHardware::UpdateMemInfo()
        {
            MEMORYSTATUSEX memStatus;
            memStatus.dwLength = sizeof(memStatus);
            GlobalMemoryStatusEx(&memStatus);
            MemInfo.TotalPhysical = memStatus.ullTotalPhys;
            MemInfo.AvailablePhysical = memStatus.ullAvailPhys;
            MemInfo.TotalPageFile = memStatus.ullTotalPageFile;
            MemInfo.AvailablePageFile = memStatus.ullAvailPageFile;
            MemInfo.TotalVirtual = memStatus.ullTotalVirtual;
            MemInfo.AvailableVirtual = memStatus.ullAvailVirtual;
            MemInfo.AvailableVirtualExtended = memStatus.ullAvailExtendedVirtual;
        }

        void MHardware::UpdateCpuInfo()
        {
            SYSTEM_INFO sysinfo;
            GetSystemInfo(&sysinfo);
            CpuInfo.Threads = sysinfo.dwNumberOfProcessors;
            CpuInfo.Cores = sysinfo.dwNumberOfProcessors / 2;
        }

        u32 MHardware::GetCurrentPID()
        {
            return GetCurrentProcessId();
        }
    }
}

#endif // WINDOWS