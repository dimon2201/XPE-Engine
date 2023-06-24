#include "app.hpp"
#include "windowing.hpp"
#include "d3d11_context.hpp"

void xpe::core::RunApp(App_Interface* app, const WindowDescriptor& desc)
{
    Window* pWindow = InitWindow(desc);
    Window window = *pWindow;

    RenderingContext_Interface* context = nullptr;
    if (desc.GPUApi == K_GPUAPI_D3D11) { context = new D3D11RenderingContext(); }
    context->Init(window);

    app->Init(pWindow, context);

    while (!ShouldWindowClose(window))
    {
        app->Update(pWindow, context);

        DefaultWindowEvents(window);
    }

    context->Free();
    FreeWindow(pWindow);
}