#pragma once

#include "windowing.hpp"

namespace xpe
{
    namespace core
    {
        class Window;
        class RenderingContext_Interface;
        class cUserInputManager;

        class App_Interface
        {
            public:
                App_Interface() {}
                ~App_Interface() {}

                virtual void Init(Window* window, RenderingContext_Interface* context, cUserInputManager* ui) = 0;
                virtual void Update(Window* window, RenderingContext_Interface* context, cUserInputManager* ui) = 0;
        };

        void RunApp(App_Interface* app, const WindowDescriptor& desc);
    }
}