#pragma once

#include "types.hpp"

namespace xpe
{
    namespace core
    {
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

            private:
                void* _instance;
                WindowDescriptor _desc;
        };

        Window* InitWindow(const WindowDescriptor& desc);
    }
}