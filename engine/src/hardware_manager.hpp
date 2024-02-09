#pragma once

namespace xpe
{
    namespace core
    {
        struct ENGINE_API sMemInfo final
        {
            usize TotalPhysical = 0;
            usize AvailablePhysical = 0;
            usize TotalPageFile = 0;
            usize AvailablePageFile = 0;
            usize TotalVirtual = 0;
            usize AvailableVirtual = 0;
            usize AvailableVirtualExtended = 0;
        };

        struct ENGINE_API sGpuInfo final
        {
            usize MaxTexture2dArray;
            usize MaxAnisotropyLevel;
            usize MaxMipLevels;
            usize MaxRenderTargetsPerStage;
            usize MaxThreadGroupsPerDimension;
            usize MaxThreadsPerGroup;
            glm::ivec3 MaxThreadGroup;
            usize MaxUnorderedAccessViews;
            bool IsConcurrentCreatesSupported;
            bool IsCommandListSupported;

            sGpuInfo();
        };

        struct ENGINE_API sCpuInfo final
        {
            u32 Cores = 1;
            u32 Threads = 1;
        };

        class ENGINE_API MHardware final
        {

        public:
            static sMemInfo MemInfo;
            static sCpuInfo CpuInfo;
            static sGpuInfo GpuInfo;

            static void UpdateMemInfo();
            static void UpdateCpuInfo();
            static void UpdateGpuInfo(void* device);
            static u32 GetCurrentPID();

        };
    }
}