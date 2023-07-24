#include <core/app.hpp>
#include <core/camera.h>

// API specific includes
#include <rendering/dx11/d3d11_context.hpp>

// Rendering features
#include <rendering/materials/material.h>
#include <rendering/lighting/light_manager.h>
#include <rendering/transforming.h>

#include <ttf/ttf_manager.hpp>

namespace xpe {

    namespace core {

        void Application::Run() {
            Config = AppConfig::Get();

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
            winDesc.Vsync = Config.Vsync;

            DeltaTime.SetFps(Config.FPS);
            CPUTime = DeltaTime;

            WindowManager::Init();
            WindowManager::InitWindow(winDesc);

            context = nullptr;

            switch (Config.GPU) {

                case AppConfig::eGPU::DX11:
                    context = new D3D11Context();
                    break;

                default:
                    LogError("App config GPU API is not supported!");
                    exit(1);

            }

            Input::Init();

            context->Init();

            ShaderManager::Init(context);
            if (Config.HotReloadShaders) {
                ShaderManager::WatchShaders("engine_shaders", true);
            }

            TextureManager::Init(context);

            MaterialManager::Init(context);

            LightManager::Init(context);

            TransformManager::Init(context);

            CameraManager::Init(context);

            ttf::TTFManager::Init();

            Init();

            while (!WindowManager::ShouldClose())
            {

                // update shader file watches if it's enabled in config
                if (Config.HotReloadShaders) {
                    ShaderManager::UpdateShaderWatches();
                }

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

            Free();

            ttf::TTFManager::Free();

            TransformManager::Free();

            LightManager::Free();

            MaterialManager::Free();

            TextureManager::Free();

            ShaderManager::Free();

            context->Free();

            Input::Free();

            WindowManager::FreeWindow();
            WindowManager::Free();

            FreeLogger();
        }

        void Application::LockFPSFromConfig() {
            if (Config.LockOnFPS && Config.FPS < DeltaTime.Fps()) {
                DeltaTime.SetFps(Config.FPS);
            }
        }

    }

}
