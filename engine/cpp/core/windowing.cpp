#include <glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <glfw3native.h>

#include <core/windowing.hpp>

#include <rendering/buffers/monitor_buffer.h>

namespace xpe {

    namespace core {

        static GLFWwindow* s_WindowHandle = nullptr;
        static GLFWmonitor* s_PrimaryMonitor = nullptr;
        static std::unordered_map<GLFWmonitor*, const GLFWvidmode*> s_MonitorTable;

        static const std::unordered_map<eCursorMode, int> s_CursorModeTable = {
                { eCursorMode::NORMAL, GLFW_CURSOR_NORMAL },
                { eCursorMode::HIDE, GLFW_CURSOR_HIDDEN },
                { eCursorMode::DISABLE, GLFW_CURSOR_DISABLED },
                { eCursorMode::CAPTURED, GLFW_CURSOR_CAPTURED }
        };

        static const std::unordered_map<eWindowTheme, u8> s_WindowThemeTable = {
                { eWindowTheme::LIGHT, 0 },
                { eWindowTheme::DARK, 20 }
        };

        Window Windowing::s_Window;

        int Windowing::s_WindowModeX = 0;
        int Windowing::s_WindowModeY = 0;
        int Windowing::s_WindowModeWidth = 0;
        int Windowing::s_WindowModeHeight = 0;

        bool Windowing::s_EnableFullscreen = false;

        void Windowing::Init() {
            LogInfo("Windowing::Init()");

            int status = glfwInit();

            if (status == GLFW_FALSE) {
                LogError("Failed to initialize GLFW");
                debug_break();
                FMT_ASSERT(false, "Failed to initialize GLFW");
            }

            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

            CreatePrimaryMonitor();

            LogInfo("Windowing initialized");
        }

        void Windowing::Free() {
            LogInfo("Windowing::Free()");
            glfwTerminate();
        }

        void Windowing::InitWindow(const WindowDescriptor &windowDescriptor) {
            LogInfo("Windowing::InitWindow()");

            s_WindowHandle = glfwCreateWindow(windowDescriptor.Width, windowDescriptor.Height, windowDescriptor.Title, nullptr, nullptr);

            s_Window.Instance = s_WindowHandle;
            s_Window.Win32Instance = glfwGetWin32Window(s_WindowHandle);
            s_Window.Descriptor = windowDescriptor;

            SetPos(windowDescriptor.X, windowDescriptor.Y);
            SetSize(windowDescriptor.Width, windowDescriptor.Height);
            SetVSync(windowDescriptor.VSync);

            LogInfo("Window initialized");
        }

        void Windowing::FreeWindow() {
            LogInfo("Windowing::FreeWindow()");
            if (s_Window.Instance != nullptr)
            {
                glfwDestroyWindow((GLFWwindow*)s_Window.Instance);
            }
        }

        const WindowDescriptor& Windowing::GetDescriptor() {
            return s_Window.Descriptor;
        }

        int Windowing::GetWidth() {
            glfwGetWindowSize(s_WindowHandle, &s_Window.Descriptor.Width, &s_Window.Descriptor.Height);
            return s_Window.Descriptor.Width;
        }

        int Windowing::GetHeight() {
            glfwGetWindowSize(s_WindowHandle, &s_Window.Descriptor.Width, &s_Window.Descriptor.Height);
            return s_Window.Descriptor.Height;
        }

        int Windowing::GetPosX() {
            glfwGetWindowPos(s_WindowHandle, &s_Window.Descriptor.X, &s_Window.Descriptor.Y);
            return s_Window.Descriptor.X;
        }

        int Windowing::GetPosY() {
            glfwGetWindowPos(s_WindowHandle, &s_Window.Descriptor.X, &s_Window.Descriptor.Y);
            return s_Window.Descriptor.Y;
        }

        const char* Windowing::GetTitle() {
            return s_Window.Descriptor.Title;
        }

        void* Windowing::GetInstance() {
            return s_Window.Instance;
        }

        void* Windowing::GetWin32Instance() {
            return s_Window.Win32Instance;
        }

        int Windowing::GetRefreshRate() {
            return s_MonitorTable[s_PrimaryMonitor]->refreshRate;
        }

        int Windowing::GetMonitorWidth() {
            return s_MonitorTable[s_PrimaryMonitor]->width;
        }

        int Windowing::GetMonitorHeight() {
            return s_MonitorTable[s_PrimaryMonitor]->height;
        }

        void Windowing::SetPos(int x, int y) {
            glfwSetWindowPos(s_WindowHandle, x, y);
            s_Window.Descriptor.X = x;
            s_Window.Descriptor.Y = y;
        }

        void Windowing::SetSize(int w, int h) {
            glfwSetWindowSize(s_WindowHandle, w, h);
            s_Window.Descriptor.Width = w;
            s_Window.Descriptor.Height = h;
        }

        bool Windowing::ShouldClose() {
            return glfwWindowShouldClose(s_WindowHandle);
        }

        void Windowing::Close() {
            glfwSetWindowShouldClose(s_WindowHandle, GLFW_TRUE);
        }

        void Windowing::PollEvents() {
            glfwPollEvents();
        }

        void Windowing::Swap() {
            glfwSwapBuffers(s_WindowHandle);
        }

        void Windowing::SetUserPointer(void* userPtr) {
            glfwSetWindowUserPointer(s_WindowHandle, userPtr);
        }

        void* Windowing::GetUserPointer() {
            return glfwGetWindowUserPointer(s_WindowHandle);
        }

        void Windowing::SetVSync(bool vsync) {
            glfwSwapInterval(vsync);
        }

        void Windowing::SetFullscreen() {
            s_WindowModeX = s_Window.Descriptor.X;
            s_WindowModeY = s_Window.Descriptor.Y;
            s_WindowModeWidth = s_Window.Descriptor.Width;
            s_WindowModeHeight = s_Window.Descriptor.Height;
            auto& mode = s_MonitorTable[s_PrimaryMonitor];
            glfwSetWindowMonitor(s_WindowHandle, s_PrimaryMonitor, 0, 0, mode->width, mode->height, mode->refreshRate);
        }

        void Windowing::SetWindowed() {
            s_Window.Descriptor.X = s_WindowModeX;
            s_Window.Descriptor.Y = s_WindowModeY;
            s_Window.Descriptor.Width = s_WindowModeWidth;
            s_Window.Descriptor.Height = s_WindowModeHeight;
            auto& refreshRate = s_MonitorTable[s_PrimaryMonitor]->refreshRate;
            glfwSetWindowMonitor(
                    s_WindowHandle,
                    nullptr,
                    s_WindowModeX, s_WindowModeY,
                    s_WindowModeWidth, s_WindowModeHeight,
                    refreshRate
            );
        }

        void Windowing::SetFullscreenWindowed() {
            s_WindowModeX = s_Window.Descriptor.X;
            s_WindowModeY = s_Window.Descriptor.Y;
            s_WindowModeWidth = s_Window.Descriptor.Width;
            s_WindowModeHeight = s_Window.Descriptor.Height;
            auto& mode = s_MonitorTable[s_PrimaryMonitor];
            glfwSetWindowMonitor(
                    s_WindowHandle,
                    nullptr,
                    0, 0,
                    mode->width, mode->height,
                    mode->refreshRate
            );
        }

        void Windowing::ToggleWindowMode() {
            s_EnableFullscreen = !s_EnableFullscreen;
            if (s_EnableFullscreen)
                SetFullscreen();
            else
                SetWindowed();
        }

        bool Windowing::IsWindowed() {
            return !s_EnableFullscreen;
        }

        void Windowing::CreatePrimaryMonitor() {
            s_PrimaryMonitor = glfwGetPrimaryMonitor();
            const GLFWvidmode* primary_mode = glfwGetVideoMode(s_PrimaryMonitor);
            s_WindowModeX = primary_mode->width / 4;
            s_WindowModeY = primary_mode->height / 4;
            s_MonitorTable[s_PrimaryMonitor] = primary_mode;

            glfwWindowHint(GLFW_RED_BITS, primary_mode->redBits);
            glfwWindowHint(GLFW_GREEN_BITS, primary_mode->greenBits);
            glfwWindowHint(GLFW_BLUE_BITS, primary_mode->blueBits);
            glfwWindowHint(GLFW_REFRESH_RATE, primary_mode->refreshRate);
        }

    }

}
