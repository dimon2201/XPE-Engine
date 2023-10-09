#ifdef WINDOWS

#define _WIN32_WINNT 0x0501
#include <Windows.h>

namespace xpe {

    namespace core {

        void Hardware::UpdateMemoryStats()
        {
            MEMORYSTATUSEX memStatus;
            memStatus.dwLength = sizeof(memStatus);
            GlobalMemoryStatusEx(&memStatus);
            s_MemoryStats.TotalPhysical = memStatus.ullTotalPhys;
            s_MemoryStats.AvailablePhysical = memStatus.ullAvailPhys;
            s_MemoryStats.TotalPageFile = memStatus.ullTotalPageFile;
            s_MemoryStats.AvailablePageFile = memStatus.ullAvailPageFile;
            s_MemoryStats.TotalVirtual = memStatus.ullTotalVirtual;
            s_MemoryStats.AvailableVirtual = memStatus.ullAvailVirtual;
            s_MemoryStats.AvailableVirtualExtended = memStatus.ullAvailExtendedVirtual;
        }

        void Hardware::UpdateCpuStats()
        {
            SYSTEM_INFO sysinfo;
            GetSystemInfo(&sysinfo);
            s_CpuStats.Cores = sysinfo.dwNumberOfProcessors;
        }

    }

}

#endif // WINDOWS