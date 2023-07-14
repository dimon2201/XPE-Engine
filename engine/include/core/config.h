#pragma once

namespace xpe {

    namespace core {

        struct ENGINE_API AppConfig final {

            enum eGPU {
                DX11 = 0,
                OPENGL = 1,
                VULKAN = 2,

                DEFAULT = DX11
            };

            static AppConfig& Get() {
                static AppConfig instance;
                return instance;
            }

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
            bool LogDebugWarnings = true;
            bool LogDebugInfos = false;

            float FPS = 165.0f;

            float LogTimeDelaySeconds = 5;

            bool LockOnFPS = true;

        };

        class ENGINE_API AppConfigReader final {

        public:
            static AppConfig Read(const char* filepath);

        private:
            static void Parse(const std::string& source, AppConfig& config);

        };

    }



}