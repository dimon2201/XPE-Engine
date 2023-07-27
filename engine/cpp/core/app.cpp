#include <core/app.hpp>
#include <core/camera.h>

// API specific includes
#include <rendering/dx11/d3d11_context.hpp>

// Rendering features
#include <rendering/materials/material.h>
#include <rendering/lighting/light_manager.h>
#include <rendering/transforming/transforming.h>
#include <rendering/camera/camera_manager.h>

// todo(cheerwizard): It would be nice to move TTF as Resource Importer into Focus editor and instead use just AssetManager for fonts
#include <ttf/ttf_manager.hpp>
using namespace xpe::ttf;

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

            CameraManager::Init(context);

            ShaderManager::Init(context);

            TextureManager::Init(context);

            MaterialManager::Init(context);

            LightManager::Init(context);

            TransformManager::Init(context);

            ttf::TTFManager::Init();

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

            ttf::TTFManager::Free();

            TransformManager::Free();

            LightManager::Free();

            MaterialManager::Free();

            TextureManager::Free();

            ShaderManager::Free();

            CameraManager::Free();

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

        void Application::InitGame() {
            m_Game->context = context;
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
