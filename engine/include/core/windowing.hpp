#pragma once

namespace xpe
{
    namespace core
    {
        class Application;

        enum class eCursorMode {
            NORMAL,
            HIDE,
            DISABLE,
            CAPTURED,

            DEFAULT = NORMAL
        };

        enum class eWindowTheme {
            LIGHT,
            DARK,

            DEFAULT = LIGHT
        };

        struct ENGINE_API WindowDescriptor
        {
            const char* Title;
            s32 Width;
            s32 Height;
            s32 X;
            s32 Y;
            Boolean Vsync;
            eCursorMode CursorMode = eCursorMode::DEFAULT;
            eWindowTheme Theme = eWindowTheme::DEFAULT;
        };

        struct ENGINE_API Window final {
            void* Instance = nullptr;
            void* Win32Instance = nullptr;
            WindowDescriptor Descriptor;
        };

        class ENGINE_API WindowManager final {

        public:
            static void Init();
            static void Free();

            static void InitWindow(const WindowDescriptor& windowDescriptor);
            static void FreeWindow();

            static const WindowDescriptor& GetDescriptor();

            static int GetWidth();
            static int GetHeight();

            static int GetPosX();
            static int GetPosY();

            static const char* GetTitle();

            static void* GetInstance();
            static void* GetWin32Instance();

            static int GetRefreshRate();
            static int GetMonitorWidth();
            static int GetMonitorHeight();

            static void SetPos(int x, int y);
            static void SetSize(int w, int h);

            static bool ShouldClose();
            static void Close();

            static void PollEvents();
            static void Swap();

            static void SetUserPointer(void* userPtr);
            static void* GetUserPointer();

            static void SetVSync(Boolean vsync);

            static void SetFullscreen();
            static void SetWindowed();
            static void SetFullscreenWindowed();
            static void ToggleWindowMode();

            static bool IsWindowed();

        private:
            static void CreatePrimaryMonitor();

        private:
            static Window s_Window;
            static int s_WindowModeX;
            static int s_WindowModeY;
            static int s_WindowModeWidth;
            static int s_WindowModeHeight;
            static bool s_EnableFullscreen;

        };

    }
}