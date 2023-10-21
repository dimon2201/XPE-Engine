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
            Memory.TotalPhysical = memStatus.ullTotalPhys;
            Memory.AvailablePhysical = memStatus.ullAvailPhys;
            Memory.TotalPageFile = memStatus.ullTotalPageFile;
            Memory.AvailablePageFile = memStatus.ullAvailPageFile;
            Memory.TotalVirtual = memStatus.ullTotalVirtual;
            Memory.AvailableVirtual = memStatus.ullAvailVirtual;
            Memory.AvailableVirtualExtended = memStatus.ullAvailExtendedVirtual;
        }

        void Hardware::UpdateCpuStats()
        {
            SYSTEM_INFO sysinfo;
            GetSystemInfo(&sysinfo);
            CPU.Cores = sysinfo.dwNumberOfProcessors;
        }

    }

}

#endif // WINDOWS