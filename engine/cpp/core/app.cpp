#include <core/app.hpp>
#include <core/windowing.hpp>
#include <core/d3d11_context.hpp>
#include <core/user_input.hpp>

void xpe::core::RunApp(App_Interface* app, const WindowDescriptor& desc)
{
    Window* pWindow = InitWindow(desc);
    Window window = *pWindow;

    RenderingContext_Interface* context = nullptr;
    cUserInputManager* ui = new cUserInputManager();

    if (desc.GPUApi == K_GPUAPI_D3D11) { context = new D3D11RenderingContext(); }

    context->Init(window);
    ui->Init(pWindow);
    app->Init(pWindow, context, ui);

    SetUserPointer(window, ui);

    while (!ShouldWindowClose(window))
    {
        ui->CaptureCursor((GLFWwindow*)window.GetInstance());
        app->Update(pWindow, context, ui);

        DefaultWindowEvents(window);
    }

    context->Free();
    FreeWindow(pWindow);
}