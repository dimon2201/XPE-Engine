#pragma once

#include <core/game.h>

namespace xpe {

    namespace core {

        using namespace xpe::render;
        using namespace xpe::control;

        class Window;

        class ENGINE_API Application {

        public:
            Time CPUTime;              // delta CPU time ms = end time - begin time
            Time DeltaTime;            // delta CurrentTime ms = end time - begin time
            Time CurrentTime;
            AppConfig Config;

        public:
            Application() {}
            ~Application() {}

        public:
            void Run();

        protected:
            void LockFPSFromConfig();

        protected:
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