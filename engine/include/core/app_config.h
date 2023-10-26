#pragma once

namespace xpe {

    namespace core {

        struct ENGINE_API AppConfig : public res::JsonObject
        {
            JsonClass(
                AppConfig,
                WinTitle,
                WinWidth,
                WinHeight,
                WinX,
                WinY,
                VSync,
                Gamma,
                Exposure,
                LogTitle,
                LogBacktrace,
                EnableGPUInfoLog,
                EnableGPUWarnLog,
                EnableGPUErrorLog,
                FPS,
                LogTimeDelaySeconds,
                LockOnFps,
                HotReloadShaders,
                MsaaSampleCount,
                EnableSSAO
            )

            string WinTitle;
            s32 WinWidth = 0;
            s32 WinHeight = 0;
            s32 WinX = 0;
            s32 WinY = 0;
            bool VSync = false;
            float Gamma = 2.2f;
            float Exposure = 1;

            string LogTitle;
            s32 LogBacktrace = 32;

            bool EnableGPUInfoLog = false;
            bool EnableGPUWarnLog = false;
            bool EnableGPUErrorLog = true;

            float FPS = 165.0f;

            float LogTimeDelaySeconds = 5;

            bool LockOnFps = true;

            bool HotReloadShaders = true;

            u32 MsaaSampleCount = 1;
            bool EnableSSAO = true;
        };

    }

}