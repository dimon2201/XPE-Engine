#include "app.hpp"
#include "windowing.hpp"

void xpe::core::RunApp(App_Interface* app, const WindowDescriptor& desc)
{
    Window* pWindow = InitWindow(desc);
    Window window = *pWindow;

    app->Init();

    while (!ShouldWindowClose(window))
    {
        app->Update();

        DefaultWindowEvents(window);
    }

    FreeWindow(pWindow);
}