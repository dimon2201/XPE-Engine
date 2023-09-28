#ifdef WINDOWS

#define _WIN32_WINNT  0x0501
#include <windows.h>

namespace xpe {

    namespace os {

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

    }

}

#endif // WINDOWS