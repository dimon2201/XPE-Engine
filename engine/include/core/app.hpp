#pragma once

#include <core/windowing.hpp>
#include <core/events.h>

namespace xpe
{
    namespace core
    {
        class Window;
        class RenderingContext_Interface;
        class cUserInputManager;

        class ENGINE_API App_Interface {

        public:
            App_Interface() {}
            ~App_Interface() {}

            virtual void Init(Window* window, RenderingContext_Interface* context, cUserInputManager* ui) = 0;
            virtual void Update(Window* window, RenderingContext_Interface* context, cUserInputManager* ui) = 0;
            virtual void Free() = 0;

        protected:
            EventBuffer<AppEventListener> m_AppEvents;

        };

        void ENGINE_API RunApp(App_Interface* app, const WindowDescriptor& desc, const LoggerDescriptor& logDesc);
    }
}