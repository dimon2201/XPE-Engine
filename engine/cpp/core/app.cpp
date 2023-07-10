#include <core/app.hpp>
#include <core/user_input.hpp>

// API specific includes
#include <rendering/dx11/d3d11_context.hpp>

#include <rendering/materials/material.h>
#include <rendering/lighting/light_manager.h>

namespace xpe {

    namespace core {

        void Application::Run() {
            LoggerDescriptor logDesc;
            logDesc.Name = AppConfig::Get().LogTitle.c_str();
            logDesc.Backtrace = AppConfig::Get().LogBacktrace;
            InitLogger(logDesc);

            WindowDescriptor winDesc;
            winDesc.Title = AppConfig::Get().WinTitle.c_str();
            winDesc.Width = AppConfig::Get().Width;
            winDesc.Height = AppConfig::Get().Height;
            winDesc.Vsync = AppConfig::Get().Vsync;
            window = InitWindow(winDesc);

            context = nullptr;

            switch (AppConfig::Get().GPU) {

                case AppConfig::eGPU::DX11:
                    context = new D3D11Context();
                    break;

                default:
                    LogError("App config GPU API is not supported!");
                    exit(1);

            }

            Input::Init(window->GetInstance());

            context->Init(*window);

            ShaderManager::Init(context);

            TextureManager::Init(context);

            MaterialManager::Init(context);

            LightManager::Init(context);

            Init();

            while (!ShouldWindowClose(*window))
            {
                static Time cpuTime; // used to measure real fps and real delta time

                time.SetFps(AppConfig::Get().FPS);

                LogTimeWithDelay(time, cpuTime)

                Timer cpuTimer(&cpuTime);
                Timer timer(&time);

                Input::CaptureCursor();

                Update();

                DefaultWindowEvents(*window);
            }

            Free();

            LightManager::Free();

            MaterialManager::Free();

            TextureManager::Free();

            ShaderManager::Free();

            context->Free();

            Input::Free();

            FreeWindow(window);

            FreeLogger();
        }

    }

}
