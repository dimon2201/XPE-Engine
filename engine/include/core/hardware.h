#pragma once

namespace xpe {

    namespace core {

        struct ENGINE_API MemoryStats final
        {
            usize TotalPhysical = 0;
            usize AvailablePhysical = 0;
            usize TotalPageFile = 0;
            usize AvailablePageFile = 0;
            usize TotalVirtual = 0;
            usize AvailableVirtual = 0;
            usize AvailableVirtualExtended = 0;
        };

        struct ENGINE_API GpuStats final
        {
            usize MaxTexture2dArray;
            usize MaxAnisotropyLevel;
            usize MaxRenderTargetsPerStage;
            bool IsConcurrentCreatesSupported;
            bool IsCommandListSupported;
        };

        struct ENGINE_API CpuStats final
        {
            u32 Cores = 1;
        };

        class ENGINE_API Hardware final {

        public:
            static MemoryStats Memory;
            static CpuStats CPU;
            static GpuStats GPU;

            static void UpdateMemoryStats();
            static void UpdateCpuStats();
            static void UpdateGpuStats(void* device);

        };

    }

}