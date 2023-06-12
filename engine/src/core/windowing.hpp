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
        };

        class Window
        {
            public:
                Window(const void* instance, const WindowDescriptor& desc);
                ~Window();

                inline void* GetInstance() { return _instance; }

            private:
                void* _instance;
                WindowDescriptor _desc;
        };

        Window* InitWindow(const WindowDescriptor& desc);
        boolean ShouldWindowClose(Window& window);
        void DefaultWindowEvents(Window& window);
        void FreeWindow(Window* window);
    }
}