#pragma once

#include <core/windowing.hpp>
#include <controls/camera.h>

namespace xpe {

    namespace core {

        using namespace xpe::render;
        using namespace xpe::control;

        class Window;

        class ENGINE_API Game {

        public:
            Context* context = nullptr;
            Time* CPUTime;
            Time* DeltaTime;
            Time* CurrentTime;
            AppConfig* Config;

        public:
            virtual void Init() {}
            virtual void Update() {}
            virtual void Free() {}

        };

    }

}

typedef xpe::core::Game* (*CreateGameFn)();