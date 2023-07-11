#pragma once

#include <core/windowing.hpp>

#include <controls/camera.h>

namespace xpe {

    namespace core {

        using namespace xpe::render;
        using namespace xpe::control;

        class Window;

        class ENGINE_API Application {

        public:
            Context* context = nullptr;
            Time dt;     // delta time ms = end time - begin time
            Time time;   // current time ms
            CameraBuffer m_CameraBuffer;

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
