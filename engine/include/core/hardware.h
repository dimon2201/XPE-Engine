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

        struct ENGINE_API VideoStats final
        {
            usize MaxTexture2dArray;
            usize MaxAnisotropyLevel;
            usize MaxRenderTargetsPerStage;
        };

        struct ENGINE_API CpuStats final
        {
            u32 Cores = 1;
        };

        class ENGINE_API Hardware final {

        public:
            static void UpdateMemoryStats();
            static MemoryStats GetMemoryStats();

            static void UpdateVideoStats();
            static VideoStats GetVideoStats();

            static void UpdateCpuStats();
            static CpuStats GetCpuStats();

        private:
            static MemoryStats s_MemoryStats;
            static VideoStats s_VideoStats;
            static CpuStats s_CpuStats;
        };

    }

}