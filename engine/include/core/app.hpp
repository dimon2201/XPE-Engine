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
            Time CPUTime;       // delta CPU time ms = end time - begin time
            Time DeltaTime;     // delta CurrentTime ms = end time - begin time
            Time CurrentTime;   // current CurrentTime ms
            CameraBuffer m_CameraBuffer;
            AppConfig Config;

        public:
            Application() {}
            ~Application() {}

        public:
            void Run();

        protected:
            virtual void Init() = 0;
            virtual void Update() = 0;
            virtual void Free() = 0;

            void LockFPSFromConfig();

        };

    }

}

xpe::core::Application* CreateApplication();
