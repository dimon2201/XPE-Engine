#pragma once

namespace xpe {

    namespace core {

        struct ENGINE_API sMemoryStats final
        {
            usize TotalPhysical = 0;
            usize AvailablePhysical = 0;
            usize TotalPageFile = 0;
            usize AvailablePageFile = 0;
            usize TotalVirtual = 0;
            usize AvailableVirtual = 0;
            usize AvailableVirtualExtended = 0;
        };

        struct ENGINE_API sGpuStats final
        {
            usize MaxTexture2dArray;
            usize MaxAnisotropyLevel;
            usize MaxRenderTargetsPerStage;
            bool IsConcurrentCreatesSupported;
            bool IsCommandListSupported;
        };

        struct ENGINE_API sCpuStats final
        {
            u32 Cores = 1;
            u32 Threads = 1;
        };

        class ENGINE_API cHardwareManager final {

        public:
            static sMemoryStats Memory;
            static sCpuStats CPU;
            static sGpuStats GPU;

            static void UpdateMemoryStats();
            static void UpdateCpuStats();
            static void UpdateGpuStats(void* device);

        };

    }

}