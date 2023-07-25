#pragma once

#include <core/game.h>

namespace xpe {

    namespace core {

        using namespace xpe::render;
        using namespace xpe::control;

        class Window;

        class ENGINE_API Application {

        public:
            Context* context = nullptr;
            Time CPUTime;              // delta CPU time ms = end time - begin time
            Time DeltaTime;            // delta CurrentTime ms = end time - begin time
            Time CurrentTime;
            CameraBuffer cameraBuffer;
            CameraBuffer cameraBuffer2D;
            AppConfig Config;

        public:
            Application() {}
            ~Application() {}

        public:
            void Run();

        protected:
            void LockFPSFromConfig();

        private:
            virtual void Init() {}
            virtual void Update() {}
            virtual void Free() {}

            virtual Game* CreateGame();

            void InitGame();

        protected:
            Game* m_Game = nullptr;

        };

    }

}

xpe::core::Application* CreateApplication();