#include <glfw3.h>
#include <cstring>

#include "windowing.hpp"
#include "app.hpp"

xpe::core::Window::Window(const void* instance, const WindowDescriptor& desc)
{
    _instance = (void*)instance;
    _desc.Width = desc.Width;
    _desc.Height = desc.Height;
    _desc.Title = desc.Title;
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
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    void* instance = (void*)glfwCreateWindow(desc.Width, desc.Height, desc.Title, nullptr, nullptr);

    return new Window(instance, desc);
}

xpe::core::boolean xpe::core::ShouldWindowClose(Window& window)
{
    return glfwWindowShouldClose((GLFWwindow*)window.GetInstance());
}

void xpe::core::DefaultWindowEvents(Window& window)
{
    glfwPollEvents();
    glfwSwapBuffers((GLFWwindow*)window.GetInstance());
}

void xpe::core::FreeWindow(Window* window)
{
    delete window;
}