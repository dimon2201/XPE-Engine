#pragma once

#include "types.hpp"

namespace xpe
{
    namespace core
    {
        class App_Interface;

        struct WindowDescriptor
        {
            u32 Width;
            u32 Height;
            const char* Title;
            uword GPUAPI;
        };

        class Window
        {
            public:
                Window(const void* instance, const WindowDescriptor& desc);
                ~Window();

                inline void* GetInstance() { return _instance; }
                inline void* GetWin32HWND() { return _win32HWND; }

            private:
                void* _instance;
                void* _win32HWND;
                WindowDescriptor _desc;
        };

        Window* InitWindow(const WindowDescriptor& desc);
        boolean ShouldWindowClose(Window& window);
        void DefaultWindowEvents(Window& window);
        void FreeWindow(Window* window);
    }
}