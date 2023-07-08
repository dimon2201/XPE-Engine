#include <core/app.hpp>
#include <core/user_input.hpp>

// API specific includes
#include <rendering/dx11/d3d11_context.hpp>
#include <rendering/dx11/d3d11_debugger.h>

#include <rendering/materials/material.h>

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
            window = InitWindow(winDesc);

            context = nullptr;
            Debugger* debugger = nullptr;

            switch (AppConfig::Get().GPU) {

                case AppConfig::eGPU::DX11:
                    context = new D3D11Context();
                    debugger = new D3D11Debugger();
                    break;

                default:
                    LogError("App config GPU API is not supported!");
                    exit(1);

            }

            context->Init(*window);

            InitDebugger(debugger, context);

            ShaderManager::Init(context);

            TextureManager::Init(context);

            MaterialManager::Init(context);

            Input::Init(window->GetInstance());

            Init();

            LogDebugMessages();

            while (!ShouldWindowClose(*window))
            {
                Timer timer(&time);

                Input::CaptureCursor();

                Update();

                DefaultWindowEvents(*window);

                LogDebugMessages();

                static int logDeltaLimit;
                logDeltaLimit++;
                if (logDeltaLimit > 2000)
                {
                    logDeltaLimit = 0;
                    LogDelta(time.Seconds());
                }
            }

            LogDebugMessages();

            Free();

            ShaderManager::Free();

            TextureManager::Free();

            MaterialManager::Free();

            FreeDebugger();

            context->Free();

            Input::Free();

            FreeWindow(window);

            FreeLogger();
        }

    }

}
