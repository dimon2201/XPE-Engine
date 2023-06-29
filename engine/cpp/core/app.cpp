#include <core/app.hpp>
#include <core/windowing.hpp>
#include <core/user_input.hpp>

// API specific includes
#include <core/d3d11_context.hpp>
#include <core/d3d11_debugger.h>

void xpe::core::RunApp(App_Interface* app, const WindowDescriptor& desc, const LoggerDescriptor& logDesc)
{
    Window* pWindow = InitWindow(desc);
    Window window = *pWindow;

    cUserInputManager* ui = new cUserInputManager();

    RenderingContext_Interface* context = nullptr;
    Debugger* debugger = nullptr;

    if (desc.GPUApi == K_GPUAPI_D3D11) {
        context = new D3D11RenderingContext();
        debugger = new D3D11Debugger();
    }

    InitLogger(logDesc);

    context->Init(window);

    InitDebugger(debugger, context);

    ui->Init(pWindow);

    app->Init(pWindow, context, ui);

    SetUserPointer(window, ui);

    LogDebugMessages();

    while (!ShouldWindowClose(window))
    {
        ui->CaptureCursor((GLFWwindow*)window.GetInstance());
        app->Update(pWindow, context, ui);

        DefaultWindowEvents(window);

        LogDebugMessages();
    }

    LogDebugMessages();

    FreeDebugger();

    context->Free();

    FreeWindow(pWindow);

    FreeLogger();

}