#pragma once

namespace xpe
{
    namespace core
    {
        class Application;

        struct ENGINE_API WindowDescriptor
        {
            const char* Title;
            s32 Width;
            s32 Height;
            Boolean Vsync;
        };

        class ENGINE_API Window
        {
            public:
                Window(const void* instance, const void* win32HWND, const WindowDescriptor& desc);
                ~Window();

                inline void* GetInstance() { return _instance; }
                inline void* GetWin32HWND() { return _win32HWND; }
                inline s32 GetWidth() { return _desc.Width; }
                inline s32 GetHeight() { return _desc.Height; }
                inline const char* GetTitle() { return _desc.Title; }

            private:
                void* _instance;
                void* _win32HWND;
                WindowDescriptor _desc;
        };

        Window* InitWindow(const WindowDescriptor& desc);
        Boolean ShouldWindowClose(Window& window);
        void DefaultWindowEvents(Window& window);
        void SetUserPointer(Window& window, void* ptr);
        void* GetUserPointer(Window& window);
        void FreeWindow(Window* window);
        ENGINE_API void CloseWindow(Window& window);
        ENGINE_API void SetVsync(Boolean vsync);
    }
}