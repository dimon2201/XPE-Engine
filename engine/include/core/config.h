#pragma once

#include <res/json.h>

namespace xpe {

    namespace core {

        struct ENGINE_API AppConfig : res::JsonObject
        {
            enum class eGPU
            {
                DX11,
                OPENGL,
                VULKAN,

                DEFAULT = DX11
            };

            JsonEnum(eGPU, {
                { eGPU::DEFAULT, "DEFAULT" },
                { eGPU::DX11, "DX11" },
                { eGPU::OPENGL, "OPENGL" },
                { eGPU::VULKAN, "VULKAN" },
            })

            JsonFields(
                AppConfig,
                WinTitle,
                WinWidth,
                WinHeight,
                WinX,
                WinY,
                Vsync,
                LogTitle,
                LogBacktrace,
                GPU,
                LogDebugErrors,
                LogDebugWarnings,
                LogDebugInfos,
                FPS,
                LogTimeDelaySeconds,
                LockOnFPS,
                HotReloadShaders
            )

            std::string WinTitle;
            s32 WinWidth = 0;
            s32 WinHeight = 0;
            s32 WinX = 0;
            s32 WinY = 0;
            Boolean Vsync = K_FALSE;

            std::string LogTitle;
            s32 LogBacktrace = 32;

            eGPU GPU = eGPU::DEFAULT;

            bool LogDebugErrors = true;
            bool LogDebugWarnings = false;
            bool LogDebugInfos = false;

            float FPS = 165.0f;

            float LogTimeDelaySeconds = 5;

            bool LockOnFPS = true;

            bool HotReloadShaders = true;

            static AppConfig& Get()
            {
                static AppConfig instance;
                return instance;
            }

        };

    }

}