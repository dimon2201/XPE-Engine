#pragma once

namespace xpe
{
    namespace core
    {
        struct ENGINE_API sAppConfig : public res::cJson
        {
            JSON_CLASS(
                sAppConfig,
                WinWidth,
                WinHeight,
                WinX,
                WinY,
                VSync,
                Fullscreen,
                Gamma,
                Exposure,
                AspectRatio,
                LogBacktrace,
                GpuInfo,
                GpuWarning,
                GpuError,
                MsaaSampleCount,
                EnableSSAO,
                AnisotropyLevel
            )

            string Title;
            s32 WinWidth = 800;
            s32 WinHeight = 600;
            s32 WinX = 200;
            s32 WinY = 150;
            bool VSync = true;
            bool Fullscreen = false;
            float Gamma = 2.2f;
            float Exposure = 1;
            float AspectRatio = 1.6f;

            s32 LogBacktrace = 32;
            bool GpuInfo = false;
            bool GpuWarning = false;
            bool GpuError = true;

            bool EnableSSAO = true;

            u32 MsaaSampleCount = 1;

            u32 AnisotropyLevel = 4;
        };
    }
}