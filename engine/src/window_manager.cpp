#include <window_manager.hpp>

#include <glfw3.h>

#ifdef WINDOWS

#define GLFW_EXPOSE_NATIVE_WIN32
#include "glfw3native.h"
#include <windef.h>

#pragma comment (lib, "Dwmapi")
#include <dwmapi.h>

#endif // WINDOWS

namespace xpe
{
    namespace core
    {
        static GLFWwindow* s_WindowHandle = nullptr;
        static GLFWmonitor* s_PrimaryMonitor = nullptr;
        static std::unordered_map<GLFWmonitor*, const GLFWvidmode*> s_MonitorTable;

        static const std::unordered_map<eCursorMode, int> k_CursorModeTable =
        {
            { eCursorMode::NORMAL, GLFW_CURSOR_NORMAL },
            { eCursorMode::HIDE, GLFW_CURSOR_HIDDEN },
            { eCursorMode::DISABLE, GLFW_CURSOR_DISABLED },
            { eCursorMode::CAPTURED, GLFW_CURSOR_CAPTURED }
        };

        static const std::unordered_map<eWindowTheme, u8> k_WindowThemeTable =
        {
            { eWindowTheme::LIGHT, 0 },
            { eWindowTheme::DARK, 20 }
        };

        sWindow MWindow::s_Window;

        int MWindow::s_WindowModeX = 0;
        int MWindow::s_WindowModeY = 0;
        int MWindow::s_WindowModeWidth = 0;
        int MWindow::s_WindowModeHeight = 0;

        bool MWindow::s_EnableFullscreen = false;

        void MWindow::Init()
        {
            LogInfo("MWindow::Init()");

            int status = glfwInit();

            if (status == GLFW_FALSE)
            {
                LogError("Failed to initialize GLFW");
                debug_break();
                FMT_ASSERT(false, "Failed to initialize GLFW");
            }

            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

            CreatePrimaryMonitor();

            MEvent::WindowFrameResized.Add("Window", 0, [](int w, int h)
            {
                s_Window.Descriptor.FrameWidth = w;
                s_Window.Descriptor.FrameHeight = h;
            });

            LogInfo("MWindow initialized");
        }

        void MWindow::Free()
        {
            LogInfo("MWindow::Free()");

            MEvent::WindowFrameResized.Remove("Window");
            glfwTerminate();
        }

        void MWindow::InitWindow(const sWindowDescriptor &windowDescriptor)
        {
            LogInfo("MWindow::InitWindow()");

            s_WindowHandle = glfwCreateWindow(
                windowDescriptor.Width,
                windowDescriptor.Height,
                windowDescriptor.Title,
                nullptr,
                nullptr
            );

            s_Window.Instance = s_WindowHandle;
            s_Window.Win32Instance = glfwGetWin32Window(s_WindowHandle);
            s_Window.Descriptor = windowDescriptor;

            SetPos(windowDescriptor.X, windowDescriptor.Y);
            SetSize(windowDescriptor.Width, windowDescriptor.Height);
            SetVSync(windowDescriptor.VSync);

            if (windowDescriptor.Fullscreen)
            {
                SetFullscreen();
            }
            else
            {
                SetWindowed();
            }

            LogInfo("Window initialized");
        }

        void MWindow::FreeWindow()
        {
            LogInfo("MWindow::FreeWindow()");
            if (s_Window.Instance != nullptr)
            {
                glfwDestroyWindow((GLFWwindow*)s_Window.Instance);
            }
        }

        const sWindowDescriptor& MWindow::GetDescriptor()
        {
            return s_Window.Descriptor;
        }

        int MWindow::GetWidth()
        {
            glfwGetWindowSize(s_WindowHandle, &s_Window.Descriptor.Width, &s_Window.Descriptor.Height);
            return s_Window.Descriptor.Width;
        }

        int MWindow::GetHeight()
        {
            glfwGetWindowSize(s_WindowHandle, &s_Window.Descriptor.Width, &s_Window.Descriptor.Height);
            return s_Window.Descriptor.Height;
        }

        int MWindow::GetFrameWidth()
        {
            return s_Window.Descriptor.FrameWidth;
        }

        int MWindow::GetFrameHeight()
        {
            return s_Window.Descriptor.FrameHeight;
        }

        int MWindow::GetPosX()
        {
            glfwGetWindowPos(s_WindowHandle, &s_Window.Descriptor.X, &s_Window.Descriptor.Y);
            return s_Window.Descriptor.X;
        }

        int MWindow::GetPosY()
        {
            glfwGetWindowPos(s_WindowHandle, &s_Window.Descriptor.X, &s_Window.Descriptor.Y);
            return s_Window.Descriptor.Y;
        }

        const char* MWindow::GetTitle()
        {
            return s_Window.Descriptor.Title;
        }

        void* MWindow::GetInstance()
        {
            return s_Window.Instance;
        }

        void* MWindow::GetWin32Instance()
        {
            return s_Window.Win32Instance;
        }

        int MWindow::GetRefreshRate()
        {
            return s_MonitorTable[s_PrimaryMonitor]->refreshRate;
        }

        int MWindow::GetMonitorWidth()
        {
            return s_MonitorTable[s_PrimaryMonitor]->width;
        }

        int MWindow::GetMonitorHeight()
        {
            return s_MonitorTable[s_PrimaryMonitor]->height;
        }

        void MWindow::SetPos(int x, int y)
        {
            glfwSetWindowPos(s_WindowHandle, x, y);
            s_Window.Descriptor.X = x;
            s_Window.Descriptor.Y = y;
        }

        void MWindow::SetSize(int w, int h)
        {
            glfwSetWindowSize(s_WindowHandle, w, h);
            s_Window.Descriptor.Width = w;
            s_Window.Descriptor.Height = h;
        }

        bool MWindow::ShouldClose()
        {
            return glfwWindowShouldClose(s_WindowHandle);
        }

        void MWindow::Close()
        {
            glfwSetWindowShouldClose(s_WindowHandle, GLFW_TRUE);
        }

        void MWindow::PollEvents()
        {
            glfwPollEvents();
        }

        void MWindow::Swap()
        {
            glfwSwapBuffers(s_WindowHandle);
        }

        void MWindow::SetUserPointer(void* userPtr)
        {
            glfwSetWindowUserPointer(s_WindowHandle, userPtr);
        }

        void* MWindow::GetUserPointer()
        {
            return glfwGetWindowUserPointer(s_WindowHandle);
        }

        void MWindow::SetVSync(bool vsync)
        {
            glfwSwapInterval(vsync);
        }

        void MWindow::SetFullscreen()
        {
            s_WindowModeX = s_Window.Descriptor.X;
            s_WindowModeY = s_Window.Descriptor.Y;
            s_WindowModeWidth = s_Window.Descriptor.Width;
            s_WindowModeHeight = s_Window.Descriptor.Height;
            auto& mode = s_MonitorTable[s_PrimaryMonitor];
            glfwSetWindowMonitor(s_WindowHandle, s_PrimaryMonitor, 0, 0, mode->width, mode->height, mode->refreshRate);
        }

        void MWindow::SetWindowed()
        {
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

        void MWindow::SetFullscreenWindowed()
        {
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

        void MWindow::ToggleWindowMode()
        {
            s_EnableFullscreen = !s_EnableFullscreen;
            if (s_EnableFullscreen)
            {
                SetFullscreen();
            }
            else
            {
                SetWindowed();
            }
        }

        bool MWindow::IsWindowed()
        {
            return !s_EnableFullscreen;
        }

        bool MWindow::IsFullscreen()
        {
            return s_EnableFullscreen;
        }

        void MWindow::CreatePrimaryMonitor()
        {
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

        void MWindow::SetTheme(eWindowTheme theme)
        {
#ifdef WINDOWS
            BOOL value = TRUE;
            DwmSetWindowAttribute((HWND) s_Window.Win32Instance, (DWORD) theme, &value, sizeof(value));
#endif
        }

        void MWindow::Minimize()
        {
            glfwIconifyWindow(s_WindowHandle);
        }

        void MWindow::HideTitleBar()
        {
            glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
        }

    }
}