#include <glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <glfw3native.h>

#include <core/windowing.hpp>

namespace xpe {

    namespace core {

        static GLFWwindow* p_WindowHandle = nullptr;

        Window WindowManager::s_Window;

        void WindowManager::Init() {
            LogInfo("WindowManager::Init()");

            int status = glfwInit();

            if (status == GLFW_FALSE) {
                LogError("Failed to initialize GLFW");
                debug_break();
                FMT_ASSERT(false, "Failed to initialize GLFW");
            }

            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

            LogInfo("WindowManager initialized");
        }

        void WindowManager::Free() {
            LogInfo("WindowManager::Free()");
            glfwTerminate();
        }

        void WindowManager::InitWindow(const WindowDescriptor &windowDescriptor) {
            LogInfo("WindowManager::InitWindow()");

            p_WindowHandle = glfwCreateWindow(windowDescriptor.Width, windowDescriptor.Height, windowDescriptor.Title, nullptr, nullptr);

            s_Window.Instance = p_WindowHandle;
            s_Window.Win32Instance = glfwGetWin32Window(p_WindowHandle);
            s_Window.Descriptor = windowDescriptor;

            SetPos(windowDescriptor.X, windowDescriptor.Y);
            SetSize(windowDescriptor.Width, windowDescriptor.Height);
            SetVSync(windowDescriptor.Vsync);

            LogInfo("Window initialized");
        }

        void WindowManager::FreeWindow() {
            LogInfo("WindowManager::FreeWindow()");
            if (s_Window.Instance != nullptr)
            {
                glfwDestroyWindow((GLFWwindow*)s_Window.Instance);
            }
        }

        const WindowDescriptor &WindowManager::GetDescriptor() {
            return s_Window.Descriptor;
        }

        int WindowManager::GetWidth() {
            glfwGetWindowSize(p_WindowHandle, &s_Window.Descriptor.Width, &s_Window.Descriptor.Height);
            return s_Window.Descriptor.Width;
        }

        int WindowManager::GetHeight() {
            glfwGetWindowSize(p_WindowHandle, &s_Window.Descriptor.Width, &s_Window.Descriptor.Height);
            return s_Window.Descriptor.Height;
        }

        int WindowManager::GetPosX() {
            glfwGetWindowPos(p_WindowHandle, &s_Window.Descriptor.X, &s_Window.Descriptor.Y);
            return s_Window.Descriptor.X;
        }

        int WindowManager::GetPosY() {
            glfwGetWindowPos(p_WindowHandle, &s_Window.Descriptor.X, &s_Window.Descriptor.Y);
            return s_Window.Descriptor.Y;
        }

        const char* WindowManager::GetTitle() {
            return s_Window.Descriptor.Title;
        }

        void* WindowManager::GetInstance() {
            return s_Window.Instance;
        }

        void* WindowManager::GetWin32Instance() {
            return s_Window.Win32Instance;
        }

        void WindowManager::SetPos(int x, int y) {
            glfwSetWindowPos(p_WindowHandle, x, y);
            s_Window.Descriptor.X = x;
            s_Window.Descriptor.Y = y;
        }

        void WindowManager::SetSize(int w, int h) {
            glfwSetWindowSize(p_WindowHandle, w, h);
            s_Window.Descriptor.Width = w;
            s_Window.Descriptor.Height = h;
        }

        bool WindowManager::ShouldClose() {
            return glfwWindowShouldClose(p_WindowHandle);
        }

        void WindowManager::Close() {
            glfwSetWindowShouldClose(p_WindowHandle, GLFW_TRUE);
        }

        void WindowManager::PollEvents() {
            glfwPollEvents();
        }

        void WindowManager::Swap() {
            glfwSwapBuffers(p_WindowHandle);
        }

        void WindowManager::SetUserPointer(void* userPtr) {
            glfwSetWindowUserPointer(p_WindowHandle, userPtr);
        }

        void* WindowManager::GetUserPointer() {
            return glfwGetWindowUserPointer(p_WindowHandle);
        }

        void WindowManager::SetVSync(Boolean vsync) {
            glfwSwapInterval(vsync);
        }

    }

}
