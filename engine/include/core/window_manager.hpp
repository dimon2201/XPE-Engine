#pragma once

namespace xpe
{
    namespace core
    {
        enum class eCursorMode
        {
            NORMAL,
            HIDE,
            DISABLE,
            CAPTURED,

            DEFAULT = NORMAL
        };

        enum class eWindowTheme
        {
            LIGHT,
            DARK,

            DEFAULT = LIGHT
        };

        struct ENGINE_API sWindowDescriptor
        {
            const char* Title;
            s32 Width;
            s32 Height;
            s32 FrameWidth;
            s32 FrameHeight;
            s32 X;
            s32 Y;
            bool VSync;
            bool Fullscreen = false;
            float Gamma = 2.2f; // 2.2 is not default for every monitor
            float Exposure = 1.0f; // level of camera exposure
            eCursorMode CursorMode = eCursorMode::DEFAULT;
            eWindowTheme Theme = eWindowTheme::DEFAULT;
        };

        struct ENGINE_API sWindow final
        {
            void* Instance = nullptr;
            void* Win32Instance = nullptr;
            sWindowDescriptor Descriptor;
        };

        class ENGINE_API cWindowManager final
        {

        public:
            static void Init();
            static void Free();

            static void InitWindow(const sWindowDescriptor& windowDescriptor);
            static void FreeWindow();

            static const sWindowDescriptor& GetDescriptor();

            static bool ShouldClose();
            static void Close();

            static void PollEvents();
            static void Swap();

            static bool IsWindowed();
            static bool IsFullscreen();

            static int GetWidth();
            static int GetHeight();

            static int GetFrameWidth();
            static int GetFrameHeight();

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

            static void SetUserPointer(void* userPtr);
            static void* GetUserPointer();

            static void SetVSync(bool vsync);

            static void SetFullscreen();
            static void SetWindowed();
            static void SetFullscreenWindowed();
            static void ToggleWindowMode();

        private:
            static void CreatePrimaryMonitor();

            static sWindow s_Window;
            static int s_WindowModeX;
            static int s_WindowModeY;
            static int s_WindowModeWidth;
            static int s_WindowModeHeight;
            static bool s_EnableFullscreen;
        };

    }
}