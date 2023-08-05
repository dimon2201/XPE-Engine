#pragma once

#include <res/json.h>

namespace xpe {

    namespace core {

        struct ENGINE_API AppConfig : res::JsonObject
        {
            JsonFields(
                AppConfig,
                WinTitle,
                WinWidth,
                WinHeight,
                WinX,
                WinY,
                VSync,
                LogTitle,
                LogBacktrace,
                DebugErrors,
                DebugWarnings,
                DebugInfo,
                FPS,
                LogTimeDelaySeconds,
                LockOnFps,
                HotReloadShaders
            )

            string WinTitle;
            s32 WinWidth = 0;
            s32 WinHeight = 0;
            s32 WinX = 0;
            s32 WinY = 0;
            bool VSync = false;

            string LogTitle;
            s32 LogBacktrace = 32;

            bool DebugErrors = true;
            bool DebugWarnings = false;
            bool DebugInfo = false;

            float FPS = 165.0f;

            float LogTimeDelaySeconds = 5;

            bool LockOnFps = true;

            bool HotReloadShaders = true;
        };

    }

}