#include <core/app.hpp>

#include <rendering/core/debugger.h>

// Rendering features
#include <rendering/lighting/light_manager.h>
#include <rendering/transforming/transforming.h>
#include <rendering/camera/camera_manager.h>

// todo(cheerwizard): It would be nice to move TTF as Resource Importer into Focus editor and instead use just AssetManager for fonts
#include <rendering/text/ttf_manager.hpp>

namespace xpe {

    namespace core {

        void Application::Run() {
            LoggerDescriptor logDesc;
            logDesc.Name = Config.LogTitle.c_str();
            logDesc.Backtrace = Config.LogBacktrace;
            InitLogger(logDesc);

            WindowDescriptor winDesc;
            winDesc.Title = Config.WinTitle.c_str();
            winDesc.Width = Config.WinWidth;
            winDesc.Height = Config.WinHeight;
            winDesc.X = Config.WinX;
            winDesc.Y = Config.WinY;
            winDesc.VSync = Config.VSync;

            DeltaTime.SetFps(Config.FPS);
            CPUTime = DeltaTime;

            WindowManager::Init();
            WindowManager::InitWindow(winDesc);

            Input::Init();

            debugger::DebugErrors = Config.DebugErrors;
            debugger::DebugWarnings = Config.DebugWarnings;
            debugger::DebugInfo = Config.DebugInfo;
            context::Init();

            CameraManager::Init();

            ShaderManager::Init();

            TextureManager::Init();

            MaterialManager::Init();

            LightManager::Init();

            TransformManager::Init();

            TTFManager::Init();

            Init();

            m_Game = CreateGame();
            InitGame();

            while (!WindowManager::ShouldClose())
            {

                // measure cpu ticks in seconds and log CPU time
#ifdef DEBUG
                static float cpuTickSec = 0;
                cpuTickSec += CPUTime.Seconds();
                if (cpuTickSec >= Config.LogTimeDelaySeconds) {
                    cpuTickSec = 0;
                    LogCpuTime(CPUTime);
                    LogMemoryPools();
                    LogStackMemory();
                }
#endif

                Timer cpuTimer(&CPUTime);
                Timer deltaTimer(&DeltaTime);

                CurrentTime = cpuTimer.GetStartTime();

                Update();

                m_Game->Update();

                WindowManager::PollEvents();
                WindowManager::Swap();

                // measure delta ticks in seconds and log delta
#ifdef DEBUG
                static float deltaTickSec = 0;
                deltaTickSec += CPUTime.Seconds();
                if (deltaTickSec >= Config.LogTimeDelaySeconds) {
                    deltaTickSec = 0;
                    LogTime(DeltaTime);
                }
#endif

            }

            m_Game->Free();

            Free();

            TTFManager::Free();

            TransformManager::Free();

            LightManager::Free();

            MaterialManager::Free();

            TextureManager::Free();

            ShaderManager::Free();

            CameraManager::Free();

            context::Free();

            Input::Free();

            WindowManager::FreeWindow();
            WindowManager::Free();

            FreeLogger();
        }

        void Application::LockFPSFromConfig() {
            if (Config.LockOnFps && Config.FPS < DeltaTime.Fps()) {
                DeltaTime.SetFps(Config.FPS);
            }
        }

        void Application::InitGame() {
            m_Game->Config = &Config;
            m_Game->CPUTime = &CPUTime;
            m_Game->DeltaTime = &DeltaTime;
            m_Game->CurrentTime = &CurrentTime;
            m_Game->Init();
        }

        Game* Application::CreateGame() {
            return new Game();
        }

    }

}
