#pragma once

#include "windowing.hpp"

namespace xpe
{
    namespace core
    {
        class App_Interface
        {
            public:
                App_Interface() {}
                ~App_Interface() {}

                virtual void Init() = 0;
                virtual void Update() = 0;
        };

        void RunApp(App_Interface* app, const WindowDescriptor& desc);
    }
}