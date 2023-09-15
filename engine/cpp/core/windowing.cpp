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

        Window WindowManager::s_Window;

        int WindowManager::s_WindowModeX = 0;
        int WindowManager::s_WindowModeY = 0;
        int WindowManager::s_WindowModeWidth = 0;
        int WindowManager::s_WindowModeHeight = 0;

        bool WindowManager::s_EnableFullscreen = false;

        render::MonitorBuffer* WindowManager::s_MonitorBuffer = nullptr;

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

            CreatePrimaryMonitor();

            LogInfo("WindowManager initialized");
        }

        void WindowManager::Free() {
            LogInfo("WindowManager::Free()");
            glfwTerminate();
        }

        void WindowManager::InitWindow(const WindowDescriptor &windowDescriptor) {
            LogInfo("WindowManager::InitWindow()");

            s_WindowHandle = glfwCreateWindow(windowDescriptor.Width, windowDescriptor.Height, windowDescriptor.Title, nullptr, nullptr);

            s_Window.Instance = s_WindowHandle;
            s_Window.Win32Instance = glfwGetWin32Window(s_WindowHandle);
            s_Window.Descriptor = windowDescriptor;

            SetPos(windowDescriptor.X, windowDescriptor.Y);
            SetSize(windowDescriptor.Width, windowDescriptor.Height);
            SetVSync(windowDescriptor.VSync);

            LogInfo("Window initialized");
        }

        void WindowManager::FreeWindow() {
            LogInfo("WindowManager::FreeWindow()");
            if (s_Window.Instance != nullptr)
            {
                glfwDestroyWindow((GLFWwindow*)s_Window.Instance);
            }
        }

        const WindowDescriptor& WindowManager::GetDescriptor() {
            return s_Window.Descriptor;
        }

        int WindowManager::GetWidth() {
            glfwGetWindowSize(s_WindowHandle, &s_Window.Descriptor.Width, &s_Window.Descriptor.Height);
            return s_Window.Descriptor.Width;
        }

        int WindowManager::GetHeight() {
            glfwGetWindowSize(s_WindowHandle, &s_Window.Descriptor.Width, &s_Window.Descriptor.Height);
            return s_Window.Descriptor.Height;
        }

        int WindowManager::GetPosX() {
            glfwGetWindowPos(s_WindowHandle, &s_Window.Descriptor.X, &s_Window.Descriptor.Y);
            return s_Window.Descriptor.X;
        }

        int WindowManager::GetPosY() {
            glfwGetWindowPos(s_WindowHandle, &s_Window.Descriptor.X, &s_Window.Descriptor.Y);
            return s_Window.Descriptor.Y;
        }

        float WindowManager::GetGamma() {
            const GLFWgammaramp* ramp = glfwGetGammaRamp(s_PrimaryMonitor);

            float sum = 0.0f;
            for (int i = 0; i < ramp->size; ++i) {
                float x = i / (float) (ramp->size - 1u);
                float y = ramp->green[i] / 65535.0f;
                sum += log2f(y) / log2f(x);
            }

            float gamma = sum / ramp->size;
            s_Window.Descriptor.Gamma = gamma;
            return gamma;
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

        int WindowManager::GetRefreshRate() {
            return s_MonitorTable[s_PrimaryMonitor]->refreshRate;
        }

        int WindowManager::GetMonitorWidth() {
            return s_MonitorTable[s_PrimaryMonitor]->width;
        }

        int WindowManager::GetMonitorHeight() {
            return s_MonitorTable[s_PrimaryMonitor]->height;
        }

        void WindowManager::SetPos(int x, int y) {
            glfwSetWindowPos(s_WindowHandle, x, y);
            s_Window.Descriptor.X = x;
            s_Window.Descriptor.Y = y;
        }

        void WindowManager::SetGamma(float gamma) {
            glfwSetGamma(s_PrimaryMonitor, gamma);
            s_Window.Descriptor.Gamma = gamma;
            s_MonitorBuffer->Item.Gamma = gamma;
            s_MonitorBuffer->Flush();
        }

        void WindowManager::SetSize(int w, int h) {
            glfwSetWindowSize(s_WindowHandle, w, h);
            s_Window.Descriptor.Width = w;
            s_Window.Descriptor.Height = h;
        }

        bool WindowManager::ShouldClose() {
            return glfwWindowShouldClose(s_WindowHandle);
        }

        void WindowManager::Close() {
            glfwSetWindowShouldClose(s_WindowHandle, GLFW_TRUE);
        }

        void WindowManager::PollEvents() {
            glfwPollEvents();
        }

        void WindowManager::Swap() {
            glfwSwapBuffers(s_WindowHandle);
        }

        void WindowManager::SetUserPointer(void* userPtr) {
            glfwSetWindowUserPointer(s_WindowHandle, userPtr);
        }

        void* WindowManager::GetUserPointer() {
            return glfwGetWindowUserPointer(s_WindowHandle);
        }

        void WindowManager::SetVSync(bool vsync) {
            glfwSwapInterval(vsync);
        }

        void WindowManager::SetFullscreen() {
            s_WindowModeX = s_Window.Descriptor.X;
            s_WindowModeY = s_Window.Descriptor.Y;
            s_WindowModeWidth = s_Window.Descriptor.Width;
            s_WindowModeHeight = s_Window.Descriptor.Height;
            auto& mode = s_MonitorTable[s_PrimaryMonitor];
            glfwSetWindowMonitor(s_WindowHandle, s_PrimaryMonitor, 0, 0, mode->width, mode->height, mode->refreshRate);
        }

        void WindowManager::SetWindowed() {
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

        void WindowManager::SetFullscreenWindowed() {
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

        void WindowManager::ToggleWindowMode() {
            s_EnableFullscreen = !s_EnableFullscreen;
            if (s_EnableFullscreen)
                SetFullscreen();
            else
                SetWindowed();
        }

        void WindowManager::SetMonitorBuffer(render::MonitorBuffer* monitorBuffer) {
            s_MonitorBuffer = monitorBuffer;
        }

        bool WindowManager::IsWindowed() {
            return !s_EnableFullscreen;
        }

        void WindowManager::CreatePrimaryMonitor() {
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
