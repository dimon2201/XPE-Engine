#include <glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <glfw3native.h>

#include <core/windowing.hpp>
#include <core/app.hpp>

xpe::core::Window::Window(const void* instance, const void* win32HWND, const WindowDescriptor& desc)
{
    _instance = (void*)instance;
    _win32HWND = (void*)win32HWND;
    _desc.Title = desc.Title;
    _desc.Width = desc.Width;
    _desc.Height = desc.Height;
    _desc.Vsync = desc.Vsync;
}

xpe::core::Window::~Window()
{
    if (_instance != nullptr)
    {
        glfwDestroyWindow((GLFWwindow*)_instance);
    }
}

xpe::core::Window* xpe::core::InitWindow(const WindowDescriptor& desc)
{
    int status = glfwInit();
    if (status == GLFW_FALSE) {
        LogError("Failed to initialize GLFW");
        debug_break();
        FMT_ASSERT(false, "Failed to initialize GLFW");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    void* instance = (void*)glfwCreateWindow(desc.Width, desc.Height, desc.Title, nullptr, nullptr);

    SetVsync(desc.Vsync);

    return new Window(instance, glfwGetWin32Window((GLFWwindow*)instance), desc);
}

xpe::core::Boolean xpe::core::ShouldWindowClose(Window& window)
{
    return glfwWindowShouldClose((GLFWwindow*)window.GetInstance());
}

void xpe::core::DefaultWindowEvents(Window& window)
{
    glfwPollEvents();
    glfwSwapBuffers((GLFWwindow*)window.GetInstance());
}

void xpe::core::SetUserPointer(Window& window, void* ptr)
{
    glfwSetWindowUserPointer((GLFWwindow*)window.GetInstance(), ptr);
}

void* xpe::core::GetUserPointer(Window& window)
{
    return glfwGetWindowUserPointer((GLFWwindow*)window.GetInstance());
}

void xpe::core::FreeWindow(Window* window)
{
    delete window;
}

void xpe::core::CloseWindow(Window& window)
{
    glfwSetWindowShouldClose((GLFWwindow*)window.GetInstance(), GLFW_TRUE);
}

void xpe::core::SetVsync(Boolean vsync)
{
    glfwSwapInterval(vsync);
}