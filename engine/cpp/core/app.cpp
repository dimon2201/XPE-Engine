#include <core/app.hpp>
#include <core/user_input.hpp>

// API specific includes
#include <rendering/dx11/d3d11_context.hpp>
#include <rendering/dx11/d3d11_debugger.h>

#include <rendering/materials/material.h>

void xpe::core::RunApp(App_Interface* app, const WindowDescriptor& desc, const LoggerDescriptor& logDesc)
{
    InitLogger(logDesc);

    Window* pWindow = InitWindow(desc);
    Window window = *pWindow;

    cUserInputManager* ui = new cUserInputManager();

    Context* context = nullptr;
    Debugger* debugger = nullptr;

    switch (EngineConfig::GPU_API) {

        case eGPU_API::DX11:
            context = new D3D11Context();
            debugger = new D3D11Debugger();
            break;

        default:
            LogError("Specified engine configuration is not supported!");
            exit(1);

    }

    context->Init(window);

    InitDebugger(debugger, context);

    ShaderManager::Init(context);

    TextureManager::Init(context);

    MaterialManager::Init(context);

    ui->Init(pWindow->GetInstance());

    app->Init(pWindow, context, ui);

    LogDebugMessages();

    while (!ShouldWindowClose(window))
    {
        ui->CaptureCursor();
        app->Update(pWindow, context, ui);

        DefaultWindowEvents(window);

        LogDebugMessages();
    }

    LogDebugMessages();

    ShaderManager::Free();

    TextureManager::Free();

    MaterialManager::Free();

    FreeDebugger();

    context->Free();

    FreeWindow(pWindow);

    FreeLogger();

}