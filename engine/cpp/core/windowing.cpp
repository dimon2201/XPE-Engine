#include <glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <glfw3native.h>

#include <core/windowing.hpp>

namespace xpe {

    namespace core {

        static GLFWwindow* window = nullptr;

        Window WindowManager::s_Window;

        void WindowManager::Init() {
            int status = glfwInit();

            if (status == GLFW_FALSE) {
                LogError("Failed to initialize GLFW");
                debug_break();
                FMT_ASSERT(false, "Failed to initialize GLFW");
            }

            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        }

        void WindowManager::Free() {
            glfwTerminate();
        }

        void WindowManager::InitWindow(const WindowDescriptor &windowDescriptor) {
            window = glfwCreateWindow(windowDescriptor.Width, windowDescriptor.Height, windowDescriptor.Title, nullptr, nullptr);

            s_Window.Instance = window;
            s_Window.Win32Instance = glfwGetWin32Window(window);
            s_Window.Descriptor = windowDescriptor;

            SetPos(windowDescriptor.X, windowDescriptor.Y);
            SetSize(windowDescriptor.Width, windowDescriptor.Height);
            SetVSync(windowDescriptor.Vsync);
        }

        void WindowManager::FreeWindow() {
            if (s_Window.Instance != nullptr)
            {
                glfwDestroyWindow((GLFWwindow*)s_Window.Instance);
            }
        }

        const WindowDescriptor &WindowManager::GetDescriptor() {
            return s_Window.Descriptor;
        }

        int WindowManager::GetWidth() {
            glfwGetWindowSize(window, &s_Window.Descriptor.Width, &s_Window.Descriptor.Height);
            return s_Window.Descriptor.Width;
        }

        int WindowManager::GetHeight() {
            glfwGetWindowSize(window, &s_Window.Descriptor.Width, &s_Window.Descriptor.Height);
            return s_Window.Descriptor.Height;
        }

        int WindowManager::GetPosX() {
            glfwGetWindowPos(window, &s_Window.Descriptor.X, &s_Window.Descriptor.Y);
            return s_Window.Descriptor.X;
        }

        int WindowManager::GetPosY() {
            glfwGetWindowPos(window, &s_Window.Descriptor.X, &s_Window.Descriptor.Y);
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
            glfwSetWindowPos(window, x, y);
            s_Window.Descriptor.X = x;
            s_Window.Descriptor.Y = y;
        }

        void WindowManager::SetSize(int w, int h) {
            glfwSetWindowSize(window, w, h);
            s_Window.Descriptor.Width = w;
            s_Window.Descriptor.Height = h;
        }

        bool WindowManager::ShouldClose() {
            return glfwWindowShouldClose(window);
        }

        void WindowManager::Close() {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }

        void WindowManager::PollEvents() {
            glfwPollEvents();
        }

        void WindowManager::Swap() {
            glfwSwapBuffers(window);
        }

        void WindowManager::SetUserPointer(void* userPtr) {
            glfwSetWindowUserPointer(window, userPtr);
        }

        void* WindowManager::GetUserPointer() {
            return glfwGetWindowUserPointer(window);
        }

        void WindowManager::SetVSync(Boolean vsync) {
            glfwSwapInterval(vsync);
        }

    }

}
