#include <glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <glfw3native.h>

#include <core/window_manager.hpp>

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

        sWindow cWindowManager::s_Window;

        int cWindowManager::s_WindowModeX = 0;
        int cWindowManager::s_WindowModeY = 0;
        int cWindowManager::s_WindowModeWidth = 0;
        int cWindowManager::s_WindowModeHeight = 0;

        bool cWindowManager::s_EnableFullscreen = false;

        render::sMonitorBuffer* cWindowManager::s_MonitorBuffer = nullptr;

        void cWindowManager::Init() {
            LogInfo("cWindowManager::Init()");

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

            LogInfo("cWindowManager initialized");
        }

        void cWindowManager::Free() {
            LogInfo("cWindowManager::Free()");
            glfwTerminate();
        }

        void cWindowManager::InitWindow(const sWindowDescriptor &windowDescriptor) {
            LogInfo("cWindowManager::InitWindow()");

            s_WindowHandle = glfwCreateWindow(windowDescriptor.Width, windowDescriptor.Height, windowDescriptor.Title, nullptr, nullptr);

            s_Window.Instance = s_WindowHandle;
            s_Window.Win32Instance = glfwGetWin32Window(s_WindowHandle);
            s_Window.Descriptor = windowDescriptor;

            SetPos(windowDescriptor.X, windowDescriptor.Y);
            SetSize(windowDescriptor.Width, windowDescriptor.Height);
            SetVSync(windowDescriptor.VSync);
            if (windowDescriptor.Fullscreen) {
                SetFullscreen();
            } else {
                SetWindowed();
            }

            LogInfo("Window initialized");
        }

        void cWindowManager::FreeWindow() {
            LogInfo("cWindowManager::FreeWindow()");
            if (s_Window.Instance != nullptr)
            {
                glfwDestroyWindow((GLFWwindow*)s_Window.Instance);
            }
        }

        const sWindowDescriptor& cWindowManager::GetDescriptor() {
            return s_Window.Descriptor;
        }

        int cWindowManager::GetWidth() {
            glfwGetWindowSize(s_WindowHandle, &s_Window.Descriptor.Width, &s_Window.Descriptor.Height);
            return s_Window.Descriptor.Width;
        }

        int cWindowManager::GetHeight() {
            glfwGetWindowSize(s_WindowHandle, &s_Window.Descriptor.Width, &s_Window.Descriptor.Height);
            return s_Window.Descriptor.Height;
        }

        int cWindowManager::GetFrameWidth()
        {
            glfwGetFramebufferSize(s_WindowHandle, &s_Window.Descriptor.FrameWidth, &s_Window.Descriptor.FrameHeight);
            return s_Window.Descriptor.FrameWidth;
        }

        int cWindowManager::GetFrameHeight()
        {
            glfwGetFramebufferSize(s_WindowHandle, &s_Window.Descriptor.FrameWidth, &s_Window.Descriptor.FrameHeight);
            return s_Window.Descriptor.FrameHeight;
        }

        int cWindowManager::GetPosX() {
            glfwGetWindowPos(s_WindowHandle, &s_Window.Descriptor.X, &s_Window.Descriptor.Y);
            return s_Window.Descriptor.X;
        }

        int cWindowManager::GetPosY() {
            glfwGetWindowPos(s_WindowHandle, &s_Window.Descriptor.X, &s_Window.Descriptor.Y);
            return s_Window.Descriptor.Y;
        }

        const char* cWindowManager::GetTitle() {
            return s_Window.Descriptor.Title;
        }

        void* cWindowManager::GetInstance() {
            return s_Window.Instance;
        }

        void* cWindowManager::GetWin32Instance() {
            return s_Window.Win32Instance;
        }

        int cWindowManager::GetRefreshRate() {
            return s_MonitorTable[s_PrimaryMonitor]->refreshRate;
        }

        int cWindowManager::GetMonitorWidth() {
            return s_MonitorTable[s_PrimaryMonitor]->width;
        }

        int cWindowManager::GetMonitorHeight() {
            return s_MonitorTable[s_PrimaryMonitor]->height;
        }

        void cWindowManager::SetPos(int x, int y) {
            glfwSetWindowPos(s_WindowHandle, x, y);
            s_Window.Descriptor.X = x;
            s_Window.Descriptor.Y = y;
        }

        void cWindowManager::SetSize(int w, int h) {
            glfwSetWindowSize(s_WindowHandle, w, h);
            s_Window.Descriptor.Width = w;
            s_Window.Descriptor.Height = h;
        }

        bool cWindowManager::ShouldClose() {
            return glfwWindowShouldClose(s_WindowHandle);
        }

        void cWindowManager::Close() {
            glfwSetWindowShouldClose(s_WindowHandle, GLFW_TRUE);
        }

        void cWindowManager::PollEvents() {
            glfwPollEvents();
        }

        void cWindowManager::Swap() {
            glfwSwapBuffers(s_WindowHandle);
        }

        void cWindowManager::SetUserPointer(void* userPtr) {
            glfwSetWindowUserPointer(s_WindowHandle, userPtr);
        }

        void* cWindowManager::GetUserPointer() {
            return glfwGetWindowUserPointer(s_WindowHandle);
        }

        void cWindowManager::SetVSync(bool vsync) {
            glfwSwapInterval(vsync);
        }

        void cWindowManager::SetFullscreen() {
            s_WindowModeX = s_Window.Descriptor.X;
            s_WindowModeY = s_Window.Descriptor.Y;
            s_WindowModeWidth = s_Window.Descriptor.Width;
            s_WindowModeHeight = s_Window.Descriptor.Height;
            auto& mode = s_MonitorTable[s_PrimaryMonitor];
            glfwSetWindowMonitor(s_WindowHandle, s_PrimaryMonitor, 0, 0, mode->width, mode->height, mode->refreshRate);
        }

        void cWindowManager::SetWindowed() {
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

        void cWindowManager::SetFullscreenWindowed() {
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

        void cWindowManager::ToggleWindowMode() {
            s_EnableFullscreen = !s_EnableFullscreen;
            if (s_EnableFullscreen)
                SetFullscreen();
            else
                SetWindowed();
        }

        bool cWindowManager::IsWindowed() {
            return !s_EnableFullscreen;
        }

        bool cWindowManager::IsFullscreen() {
            return s_EnableFullscreen;
        }

        void cWindowManager::CreatePrimaryMonitor() {
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

        void cWindowManager::SetMonitorBuffer(render::sMonitorBuffer *monitorBuffer) {
            s_MonitorBuffer = monitorBuffer;
        }

        void cWindowManager::SetExposure(float exposure) {
            s_MonitorBuffer->Item.Exposure = exposure;
            s_MonitorBuffer->Flush();
        }

        void cWindowManager::SetGamma(float gamma) {
            s_MonitorBuffer->Item.Gamma = gamma;
            s_MonitorBuffer->Flush();
        }

        float cWindowManager::GetExposure() {
            return s_MonitorBuffer->Item.Exposure;
        }

        float cWindowManager::GetGamma() {
            return s_MonitorBuffer->Item.Gamma;
        }

    }

}
