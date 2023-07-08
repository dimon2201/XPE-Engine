#pragma once

#include <core/windowing.hpp>
#include <core/events.h>

#include <rendering/context.hpp>

namespace xpe {

    namespace core {

        using namespace xpe::render;

        class Window;

        class ENGINE_API Application {

        public:
            Context* context = nullptr;
            Window* window = nullptr;
            Time time;

        public:
            Application() {}
            ~Application() {}

        public:
            void Run();

        protected:
            virtual void Init() = 0;
            virtual void Update() = 0;
            virtual void Free() = 0;

        };

    }

}

xpe::core::Application* CreateApplication();
