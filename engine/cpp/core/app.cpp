#include <core/app.hpp>

// API specific includes
#include <rendering/dx11/d3d11_context.hpp>

// Rendering features
#include <rendering/materials/material.h>
#include <rendering/lighting/light_manager.h>
#include <rendering/transforming.h>

namespace xpe {

    namespace core {

        void Application::Run() {
            AppConfig appConfig = AppConfig::Get();

            LoggerDescriptor logDesc;
            logDesc.Name = appConfig.LogTitle.c_str();
            logDesc.Backtrace = appConfig.LogBacktrace;
            InitLogger(logDesc);

            WindowDescriptor winDesc;
            winDesc.Title = appConfig.WinTitle.c_str();
            winDesc.Width = appConfig.WinWidth;
            winDesc.Height = appConfig.WinHeight;
            winDesc.X = appConfig.WinX;
            winDesc.Y = appConfig.WinY;
            winDesc.Vsync = appConfig.Vsync;

            WindowManager::Init();
            WindowManager::InitWindow(winDesc);

            context = nullptr;

            switch (appConfig.GPU) {

                case AppConfig::eGPU::DX11:
                    context = new D3D11Context();
                    break;

                default:
                    LogError("App config GPU API is not supported!");
                    exit(1);

            }

            Input::Init();

            context->Init();

            m_CameraBuffer = CameraBuffer(context, 1); // by default, we have a single camera in memory

            ShaderManager::Init(context);

            TextureManager::Init(context);

            MaterialManager::Init(context);

            LightManager::Init(context);

            TransformManager::Init(context);

            Init();

            while (!WindowManager::ShouldClose())
            {
                static Time cpuTime; // used to measure real fps and real delta time

                dt.SetFps(AppConfig::Get().FPS);

                LogTimeWithDelay(dt, cpuTime)

                Timer cpuTimer(&cpuTime);
                Timer timer(&dt);
                time = timer.GetStartTime();

                Update();

                WindowManager::PollEvents();
                WindowManager::Swap();
            }

            Free();

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

    }

}
