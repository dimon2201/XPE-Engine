#pragma once

#include <core/observers.hpp>

namespace xpe {

    namespace core {

        struct ENGINE_API sAppConfig : public res::cJson
        {
            JSON_CLASS(
                sAppConfig,
                WinTitle,
                WinWidth,
                WinHeight,
                WinX,
                WinY,
                VSync,
                Fullscreen,
                Gamma,
                Exposure,
                AspectRatio.Value,
                LogTitle,
                LogBacktrace,
                EnableGPUInfoLog,
                EnableGPUWarnLog,
                EnableGPUErrorLog,
                FPS,
                LogTimeDelaySeconds,
                LockOnFps,
                HotReloadShaders,
                MsaaSampleCount.Value,
                EnableSSAO
            )

            string WinTitle;
            s32 WinWidth = 0;
            s32 WinHeight = 0;
            s32 WinX = 0;
            s32 WinY = 0;
            bool VSync = false;
            bool Fullscreen = false;
            float Gamma = 2.2f;
            float Exposure = 1;
            sObserverWindowRatio AspectRatio = 1.6f;

            string LogTitle;
            s32 LogBacktrace = 32;

            bool EnableGPUInfoLog = false;
            bool EnableGPUWarnLog = false;
            bool EnableGPUErrorLog = true;

            float FPS = 165.0f;

            float LogTimeDelaySeconds = 5;

            bool LockOnFps = true;

            bool HotReloadShaders = true;

            bool EnableSSAO = true;

            sObserverSampleCount MsaaSampleCount = 1;
        };

    }

}