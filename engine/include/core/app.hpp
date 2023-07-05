#pragma once

#include <core/windowing.hpp>
#include <core/events.h>

#include <rendering/context.hpp>

namespace xpe {

    namespace core {

        using namespace xpe::render;

        class Window;
        class cUserInputManager;

        class ENGINE_API App_Interface {

        public:
            App_Interface() {}
            ~App_Interface() {}

            virtual void Init(Window* window, Context* context, cUserInputManager* ui) = 0;
            virtual void Update(Window* window, Context* context, cUserInputManager* ui) = 0;
            virtual void Free() = 0;

        protected:
            EventBuffer<AppEventListener> m_AppEvents;

        };

        void ENGINE_API RunApp(App_Interface* app, const WindowDescriptor& desc, const LoggerDescriptor& logDesc);
    }

}
